
#include "root_view.h"

#include <algorithm>

#include "base/logging.h"

#include "message_framework/message_loop.h"

#include "gfx/canvas_gdiplus.h"

#include "../app/keyboard_codes_win.h"
#include "../dragdrop/drag_drop_types.h"
#include "../dragdrop/drag_source.h"
#include "../dragdrop/os_exchange_data_provider_win.h"
#include "../focus/view_storage.h"
#include "../layout/fill_layout.h"
#include "widget.h"

namespace view
{

    /////////////////////////////////////////////////////////////////////////////
    //
    // A Task to trigger non urgent painting.
    //
    /////////////////////////////////////////////////////////////////////////////
    class PaintTask : public Task
    {
    public:
        explicit PaintTask(RootView* target) : root_view_(target) {}

        ~PaintTask() {}

        void Cancel()
        {
            root_view_ = NULL;
        }

        void Run()
        {
            if(root_view_)
            {
                root_view_->PaintNow();
            }
        }

    private:
        // The target root view.
        RootView* root_view_;

        DISALLOW_COPY_AND_ASSIGN(PaintTask);
    };

    const char RootView::kViewClassName[] = "view/RootView";

    /////////////////////////////////////////////////////////////////////////////
    //
    // RootView - constructors, destructors, initialization
    //
    /////////////////////////////////////////////////////////////////////////////

    RootView::RootView(Widget* widget)
        : mouse_pressed_handler_(NULL),
        mouse_move_handler_(NULL),
        last_click_handler_(NULL),
        widget_(widget),
        focus_search_(this, false, false),
        invalid_rect_urgent_(false),
        pending_paint_task_(NULL),
        paint_task_needed_(false),
        explicit_mouse_handler_(false),
        previous_cursor_(NULL),
        default_keyboard_handler_(NULL),
        focus_on_mouse_pressed_(false),
        ignore_set_focus_calls_(false),
        focus_traversable_parent_(NULL),
        focus_traversable_parent_view_(NULL),
        drag_view_(NULL)
#ifndef NDEBUG
        ,
        is_processing_paint_(false)
#endif
    {}

    RootView::~RootView()
    {
        // If we have children remove them explicitly so to make sure a remove
        // notification is sent for each one of them.
        if(!child_views_.empty())
        {
            RemoveAllChildViews(true);
        }

        if(pending_paint_task_)
        {
            pending_paint_task_->Cancel(); // Ensure we're not called any more.
        }
    }

    void RootView::SetContentsView(View* contents_view)
    {
        DCHECK(contents_view && GetWidget()->GetNativeView()) <<
            "Can't be called until after the native view is created!";
        // The ContentsView must be set up _after_ the window is created so that its
        // Widget pointer is valid.
        SetLayoutManager(new FillLayout);
        if(GetChildViewCount() != 0)
        {
            RemoveAllChildViews(true);
        }
        AddChildView(contents_view);

        // Force a layout now, since the attached hierarchy won't be ready for the
        // containing window's bounds. Note that we call Layout directly rather than
        // calling the widget's size changed handler, since the RootView's bounds may
        // not have changed, which will cause the Layout not to be done otherwise.
        Layout();
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    // RootView - layout, painting
    //
    /////////////////////////////////////////////////////////////////////////////

    void RootView::SchedulePaint(const gfx::Rect& r, bool urgent)
    {
        // If there is an existing invalid rect, add the union of the scheduled
        // rect with the invalid rect. This could be optimized further if
        // necessary.
        if(invalid_rect_.IsEmpty())
        {
            invalid_rect_ = r;
        }
        else
        {
            invalid_rect_ = invalid_rect_.Union(r);
        }

        if(urgent || invalid_rect_urgent_)
        {
            invalid_rect_urgent_ = true;
        }
        else
        {
            if(!pending_paint_task_)
            {
                pending_paint_task_ = new PaintTask(this);
                MessageLoop::current()->PostTask(pending_paint_task_);
            }
            paint_task_needed_ = true;
        }
    }

    void RootView::SchedulePaint()
    {
        View::SchedulePaint();
    }

#ifndef NDEBUG
    // Sets the value of RootView's |is_processing_paint_| member to true as long
    // as ProcessPaint is being called. Sets it to |false| when it returns.
    class ScopedProcessingPaint
    {
    public:
        explicit ScopedProcessingPaint(bool* is_processing_paint)
            : is_processing_paint_(is_processing_paint)
        {
            *is_processing_paint_ = true;
        }

        ~ScopedProcessingPaint()
        {
            *is_processing_paint_ = false;
        }
    private:
        bool* is_processing_paint_;

        DISALLOW_COPY_AND_ASSIGN(ScopedProcessingPaint);
    };
#endif

    void RootView::ProcessPaint(gfx::Canvas* canvas)
    {
#ifndef NDEBUG
        ScopedProcessingPaint processing_paint(&is_processing_paint_);
#endif

        // Clip the invalid rect to our bounds. If a view is in a scrollview
        // it could be a lot larger
        invalid_rect_ = GetScheduledPaintRectConstrainedToSize();

        if(invalid_rect_.IsEmpty())
        {
            return;
        }

        // Clear the background.
        canvas->AsCanvasGdiplus()->Clear(Gdiplus::Color::Black);

        // Save the current transforms.
        canvas->Save();

        // Set the clip rect according to the invalid rect.
        int clip_x = invalid_rect_.x() + x();
        int clip_y = invalid_rect_.y() + y();
        canvas->ClipRectInt(clip_x, clip_y, invalid_rect_.width(),
            invalid_rect_.height());

        // Paint the tree
        View::ProcessPaint(canvas);

        // Restore the previous transform
        canvas->Restore();

        ClearPaintRect();
    }

    void RootView::PaintNow()
    {
        if(pending_paint_task_)
        {
            pending_paint_task_->Cancel();
            pending_paint_task_ = NULL;
        }
        if(!paint_task_needed_)
        {
            return;
        }
        Widget* widget = GetWidget();
        if(widget)
        {
            widget->PaintNow(invalid_rect_);
        }
    }

    bool RootView::NeedsPainting(bool urgent)
    {
        bool has_invalid_rect = !invalid_rect_.IsEmpty();
        if(urgent)
        {
            if(invalid_rect_urgent_)
            {
                return has_invalid_rect;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return has_invalid_rect;
        }
    }

    const gfx::Rect& RootView::GetScheduledPaintRect()
    {
        return invalid_rect_;
    }

    gfx::Rect RootView::GetScheduledPaintRectConstrainedToSize()
    {
        if(invalid_rect_.IsEmpty())
        {
            return invalid_rect_;
        }

        return invalid_rect_.Intersect(GetLocalBounds(true));
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    // RootView - tree
    //
    /////////////////////////////////////////////////////////////////////////////

    Widget* RootView::GetWidget() const
    {
        return widget_;
    }

    void RootView::NotifyThemeChanged()
    {
        View::PropagateThemeChanged();
    }

    void RootView::NotifyLocaleChanged()
    {
        View::PropagateLocaleChanged();
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    // RootView - event dispatch and propagation
    //
    /////////////////////////////////////////////////////////////////////////////

    void RootView::ViewHierarchyChanged(bool is_add, View* parent, View* child)
    {
        if(!is_add)
        {
            if(!explicit_mouse_handler_ && mouse_pressed_handler_==child)
            {
                mouse_pressed_handler_ = NULL;
            }

            if(widget_)
            {
                widget_->ViewHierarchyChanged(is_add, parent, child);
            }

            if(mouse_move_handler_ == child)
            {
                mouse_move_handler_ = NULL;
            }

            if(GetFocusedView() == child)
            {
                FocusView(NULL);
            }

            if(child == drag_view_)
            {
                drag_view_ = NULL;
            }

            if(default_keyboard_handler_ == child)
            {
                default_keyboard_handler_ = NULL;
            }

            FocusManager* focus_manager = widget_->GetFocusManager();
            // An unparanted RootView does not have a FocusManager.
            if(focus_manager)
            {
                focus_manager->ViewRemoved(parent, child);
            }

            ViewStorage::GetSharedInstance()->ViewRemoved(parent, child);
        }
    }

    void RootView::SetFocusOnMousePressed(bool f)
    {
        focus_on_mouse_pressed_ = f;
    }

    bool RootView::OnMousePressed(const MouseEvent& e)
    {
        // This function does not normally handle non-client messages except for
        // non-client double-clicks. Actually, all double-clicks are special as the
        // are formed from a single-click followed by a double-click event. When the
        // double-click event lands on a different view than its single-click part,
        // we transform it into a single-click which prevents odd things.
        if((e.GetFlags()&MouseEvent::EF_IS_NON_CLIENT) &&
            !(e.GetFlags()&MouseEvent::EF_IS_DOUBLE_CLICK))
        {
            last_click_handler_ = NULL;
            return false;
        }

        UpdateCursor(e);
        SetMouseLocationAndFlags(e);

        // If mouse_pressed_handler_ is non null, we are currently processing
        // a pressed -> drag -> released session. In that case we send the
        // event to mouse_pressed_handler_
        if(mouse_pressed_handler_)
        {
            MouseEvent mouse_pressed_event(e, this, mouse_pressed_handler_);
            drag_info.Reset();
            mouse_pressed_handler_->ProcessMousePressed(mouse_pressed_event,
                &drag_info);
            return true;
        }
        DCHECK(!explicit_mouse_handler_);

        bool hit_disabled_view = false;
        // Walk up the tree until we find a view that wants the mouse event.
        for(mouse_pressed_handler_=GetViewForPoint(e.location());
            mouse_pressed_handler_&&(mouse_pressed_handler_ != this);
            mouse_pressed_handler_=mouse_pressed_handler_->GetParent())
        {
            if(!mouse_pressed_handler_->IsEnabled())
            {
                // Disabled views should eat events instead of propagating them upwards.
                hit_disabled_view = true;
                break;
            }

            // See if this view wants to handle the mouse press.
            MouseEvent mouse_pressed_event(e, this, mouse_pressed_handler_);

            // Remove the double-click flag if the handler is different than the
            // one which got the first click part of the double-click.
            if(mouse_pressed_handler_ != last_click_handler_)
            {
                mouse_pressed_event.set_flags(e.GetFlags() &
                    ~MouseEvent::EF_IS_DOUBLE_CLICK);
            }

            drag_info.Reset();
            bool handled = mouse_pressed_handler_->ProcessMousePressed(
                mouse_pressed_event, &drag_info);

            // The view could have removed itself from the tree when handling
            // OnMousePressed().  In this case, the removal notification will have
            // reset mouse_pressed_handler_ to NULL out from under us.  Detect this
            // case and stop.  (See comments in view.h.)
            //
            // NOTE: Don't return true here, because we don't want the frame to
            // forward future events to us when there's no handler.
            if(!mouse_pressed_handler_)
            {
                break;
            }

            // If the view handled the event, leave mouse_pressed_handler_ set and
            // return true, which will cause subsequent drag/release events to get
            // forwarded to that view.
            if(handled)
            {
                last_click_handler_ = mouse_pressed_handler_;
                return true;
            }
        }

        // Reset mouse_pressed_handler_ to indicate that no processing is occurring.
        mouse_pressed_handler_ = NULL;

        if(focus_on_mouse_pressed_)
        {
            HWND hwnd = GetWidget()->GetNativeView();
            if(::GetFocus() != hwnd)
            {
                ::SetFocus(hwnd);
            }
        }

        // In the event that a double-click is not handled after traversing the
        // entire hierarchy (even as a single-click when sent to a different view),
        // it must be marked as handled to avoid anything happening from default
        // processing if it the first click-part was handled by us.
        if(last_click_handler_ && e.GetFlags()&MouseEvent::EF_IS_DOUBLE_CLICK)
        {
            hit_disabled_view = true;
        }

        last_click_handler_ = NULL;
        return hit_disabled_view;
    }

    bool RootView::ConvertPointToMouseHandler(const gfx::Point& l,
        gfx::Point* p)
    {
        // If the mouse_handler was set explicitly, we need to keep
        // sending events even if it was reparented in a different
        // window. (a non explicit mouse handler is automatically
        // cleared when the control is removed from the hierarchy)
        if(explicit_mouse_handler_)
        {
            if(mouse_pressed_handler_->GetWidget())
            {
                *p = l;
                ConvertPointToScreen(this, p);
                ConvertPointToView(NULL, mouse_pressed_handler_, p);
            }
            else
            {
                // If the mouse_pressed_handler_ is not connected, we send the
                // event in screen coordinate system
                *p = l;
                ConvertPointToScreen(this, p);
                return true;
            }
        }
        else
        {
            *p = l;
            ConvertPointToView(this, mouse_pressed_handler_, p);
        }
        return true;
    }

    void RootView::UpdateCursor(const MouseEvent& e)
    {
        HCURSOR cursor = NULL;
        View* v = GetViewForPoint(e.location());
        if(v && v!=this)
        {
            gfx::Point l(e.location());
            View::ConvertPointToView(this, v, &l);
            cursor = v->GetCursorForPoint(e.GetType(), l);
        }
        SetActiveCursor(cursor);
    }

    bool RootView::OnMouseDragged(const MouseEvent& e)
    {
        UpdateCursor(e);

        if(mouse_pressed_handler_)
        {
            SetMouseLocationAndFlags(e);

            gfx::Point p;
            ConvertPointToMouseHandler(e.location(), &p);
            MouseEvent mouse_event(e.GetType(), p.x(), p.y(), e.GetFlags());
            return mouse_pressed_handler_->ProcessMouseDragged(mouse_event, &drag_info);
        }
        return false;
    }

    void RootView::OnMouseReleased(const MouseEvent& e, bool canceled)
    {
        UpdateCursor(e);

        if(mouse_pressed_handler_)
        {
            gfx::Point p;
            ConvertPointToMouseHandler(e.location(), &p);
            MouseEvent mouse_released(e.GetType(), p.x(), p.y(), e.GetFlags());
            // We allow the view to delete us from ProcessMouseReleased. As such,
            // configure state such that we're done first, then call View.
            View* mouse_pressed_handler = mouse_pressed_handler_;
            mouse_pressed_handler_ = NULL;
            explicit_mouse_handler_ = false;
            mouse_pressed_handler->ProcessMouseReleased(mouse_released, canceled);
            // WARNING: we may have been deleted.
        }
    }

    void RootView::OnMouseMoved(const MouseEvent& e)
    {
        View* v = GetViewForPoint(e.location());
        // Find the first enabled view, or the existing move handler, whichever comes
        // first.  The check for the existing handler is because if a view becomes
        // disabled while handling moves, it's wrong to suddenly send ET_MOUSE_EXITED
        // and ET_MOUSE_ENTERED events, because the mouse hasn't actually exited yet.
        while(v && !v->IsEnabled() && (v!=mouse_move_handler_))
        {
            v = v->GetParent();
        }
        if(v && v!=this)
        {
            if(v != mouse_move_handler_)
            {
                if(mouse_move_handler_ != NULL)
                {
                    MouseEvent exited_event(Event::ET_MOUSE_EXITED, 0, 0, 0);
                    mouse_move_handler_->OnMouseExited(exited_event);
                }

                mouse_move_handler_ = v;

                MouseEvent entered_event(Event::ET_MOUSE_ENTERED,
                    this,
                    mouse_move_handler_,
                    e.location(),
                    0);
                mouse_move_handler_->OnMouseEntered(entered_event);
            }
            MouseEvent moved_event(Event::ET_MOUSE_MOVED,
                this,
                mouse_move_handler_,
                e.location(),
                0);
            mouse_move_handler_->OnMouseMoved(moved_event);

            HCURSOR cursor = mouse_move_handler_->GetCursorForPoint(
                moved_event.GetType(), moved_event.location());
            SetActiveCursor(cursor);
        }
        else if(mouse_move_handler_ != NULL)
        {
            MouseEvent exited_event(Event::ET_MOUSE_EXITED, 0, 0, 0);
            mouse_move_handler_->OnMouseExited(exited_event);
            SetActiveCursor(NULL);
        }
    }

    void RootView::ProcessOnMouseExited()
    {
        if(mouse_move_handler_ != NULL)
        {
            MouseEvent exited_event(Event::ET_MOUSE_EXITED, 0, 0, 0);
            mouse_move_handler_->OnMouseExited(exited_event);
            mouse_move_handler_ = NULL;
        }
    }

    void RootView::SetMouseHandler(View *new_mh)
    {
        // If we're clearing the mouse handler, clear explicit_mouse_handler as well.
        explicit_mouse_handler_ = (new_mh != NULL);
        mouse_pressed_handler_ = new_mh;
    }

    void RootView::ProcessMouseDragCanceled()
    {
        if(mouse_pressed_handler_)
        {
            // Synthesize a release event.
            MouseEvent release_event(Event::ET_MOUSE_RELEASED, last_mouse_event_x_,
                last_mouse_event_y_, last_mouse_event_flags_);
            OnMouseReleased(release_event, true);
        }
    }

    void RootView::FocusView(View* view)
    {
        if(view != GetFocusedView())
        {
            FocusManager* focus_manager = GetFocusManager();
            // TODO(jcampan): This fails under WidgetGtk with TYPE_CHILD.
            // (see http://crbug.com/21335) Reenable DCHECK and
            // verify GetFocusManager works as expecte.
            DCHECK(focus_manager) << "No Focus Manager for Window " <<
                (GetWidget() ? GetWidget()->GetNativeView() : 0);
            if(!focus_manager)
            {
                return;
            }
            focus_manager->SetFocusedView(view);
        }
    }

    View* RootView::GetFocusedView()
    {
        FocusManager* focus_manager = GetFocusManager();
        if(!focus_manager)
        {
            // We may not have a FocusManager when the window that contains us is being
            // deleted. Sadly we cannot wait for the window to be destroyed before we
            // remove the FocusManager (see xp_frame.cc for more info).
            return NULL;
        }

        // Make sure the focused view belongs to this RootView's view hierarchy.
        View* view = focus_manager->GetFocusedView();
        if(view && (view->GetRootView()==this))
        {
            return view;
        }
        return NULL;
    }

    FocusSearch* RootView::GetFocusSearch()
    {
        return &focus_search_;
    }

    FocusTraversable* RootView::GetFocusTraversableParent()
    {
        return focus_traversable_parent_;
    }

    void RootView::SetFocusTraversableParent(FocusTraversable* focus_traversable)
    {
        DCHECK(focus_traversable != this);
        focus_traversable_parent_ = focus_traversable;
    }

    View* RootView::GetFocusTraversableParentView()
    {
        return focus_traversable_parent_view_;
    }

    void RootView::SetFocusTraversableParentView(View* view)
    {
        focus_traversable_parent_view_ = view;
    }

    void RootView::NotifyNativeViewHierarchyChanged(bool attached,
        HWND native_view)
    {
        PropagateNativeViewHierarchyChanged(attached, native_view, this);
    }

    bool RootView::ProcessKeyEvent(const KeyEvent& event)
    {
        bool consumed = false;

        View* v = GetFocusedView();
        // Special case to handle right-click context menus triggered by the
        // keyboard.
        if(v && v->IsEnabled() && ((event.GetKeyCode()==VKEY_APPS) ||
            (event.GetKeyCode()==VKEY_F10 && event.IsShiftDown())))
        {
            v->ShowContextMenu(v->GetKeyboardContextMenuLocation(), false);
            return true;
        }
        for(; v&&v!=this&&!consumed; v=v->GetParent())
        {
            consumed = (event.GetType() == Event::ET_KEY_PRESSED) ?
                v->OnKeyPressed(event) : v->OnKeyReleased(event);
        }

        if(!consumed && default_keyboard_handler_)
        {
            consumed = (event.GetType() == Event::ET_KEY_PRESSED) ?
                default_keyboard_handler_->OnKeyPressed(event) :
            default_keyboard_handler_->OnKeyReleased(event);
        }

        return consumed;
    }

    bool RootView::ProcessMouseWheelEvent(const MouseWheelEvent& e)
    {
        View* v;
        bool consumed = false;
        if(GetFocusedView())
        {
            for(v=GetFocusedView(); v&&v!=this&&!consumed; v=v->GetParent())
            {
                consumed = v->OnMouseWheel(e);
            }
        }

        if(!consumed && default_keyboard_handler_)
        {
            consumed = default_keyboard_handler_->OnMouseWheel(e);
        }
        return consumed;
    }

    void RootView::SetDefaultKeyboardHandler(View* v)
    {
        default_keyboard_handler_ = v;
    }

    bool RootView::IsVisibleInRootView() const
    {
        return IsVisible();
    }

    void RootView::ViewBoundsChanged(View* view, bool size_changed,
        bool position_changed)
    {
        DCHECK(view && (size_changed || position_changed));
        if(!view->descendants_to_notify_.get())
        {
            return;
        }

        for(std::vector<View*>::iterator i=view->descendants_to_notify_->begin();
            i!=view->descendants_to_notify_->end(); ++i)
        {
            (*i)->VisibleBoundsInRootChanged();
        }
    }

    void RootView::RegisterViewForVisibleBoundsNotification(View* view)
    {
        DCHECK(view);
        if(view->registered_for_visible_bounds_notification_)
        {
            return;
        }
        view->registered_for_visible_bounds_notification_ = true;
        View* ancestor = view->GetParent();
        while(ancestor)
        {
            ancestor->AddDescendantToNotify(view);
            ancestor = ancestor->GetParent();
        }
    }

    void RootView::UnregisterViewForVisibleBoundsNotification(View* view)
    {
        DCHECK(view);
        if(!view->registered_for_visible_bounds_notification_)
        {
            return;
        }
        view->registered_for_visible_bounds_notification_ = false;
        View* ancestor = view->GetParent();
        while(ancestor)
        {
            ancestor->RemoveDescendantToNotify(view);
            ancestor = ancestor->GetParent();
        }
    }

    void RootView::SetMouseLocationAndFlags(const MouseEvent& e)
    {
        last_mouse_event_flags_ = e.GetFlags();
        last_mouse_event_x_ = e.x();
        last_mouse_event_y_ = e.y();
    }

    std::string RootView::GetClassName() const
    {
        return kViewClassName;
    }

    void RootView::ClearPaintRect()
    {
        invalid_rect_.SetRect(0, 0, 0, 0);

        // This painting has been done. Reset the urgent flag.
        invalid_rect_urgent_ = false;

        // If a pending_paint_task_ does Run(), we don't need to do anything.
        paint_task_needed_ = false;
    }

    void RootView::OnPaint(HWND hwnd)
    {
        gfx::Rect original_dirty_region = GetScheduledPaintRectConstrainedToSize();
        if(!original_dirty_region.IsEmpty())
        {
            // Invoke InvalidateRect so that the dirty region of the window includes the
            // region we need to paint. If we didn't do this and the region didn't
            // include the dirty region, ProcessPaint would incorrectly mark everything
            // as clean. This can happen if a WM_PAINT is generated by the system before
            // the InvokeLater schedule by RootView is processed.
            RECT win_version = original_dirty_region.ToRECT();
            InvalidateRect(hwnd, &win_version, FALSE);
        }

        scoped_ptr<gfx::CanvasPaint> canvas(
            gfx::CanvasPaint::CreateCanvasPaint(hwnd));
        if(!canvas->IsValid())
        {
            SchedulePaint(canvas->GetInvalidRect(), false);
            if(NeedsPainting(false))
            {
                ProcessPaint(canvas->AsCanvas());
            }
        }
    }

    void RootView::StartDragForViewFromMouseEvent(View* view,
        const OSExchangeData& data, int operation)
    {
        // NOTE: view may be null.
        drag_view_ = view;
        scoped_refptr<DragSource> drag_source(new DragSource);
        DWORD effects;
        DoDragDrop(OSExchangeDataProviderWin::GetIDataObject(data), drag_source,
            DragDropTypes::DragOperationToDropEffect(operation), &effects);
        // If the view is removed during the drag operation, drag_view_ is set to
        // NULL.
        if(view && drag_view_==view)
        {
            View* drag_view = drag_view_;
            drag_view_ = NULL;
            drag_view->OnDragDone();
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    //
    // RootView - accessibility
    //
    /////////////////////////////////////////////////////////////////////////////

    AccessibilityTypes::Role RootView::GetAccessibleRole()
    {
        return AccessibilityTypes::ROLE_APPLICATION;
    }

    View* RootView::GetDragView()
    {
        return drag_view_;
    }

    void RootView::SetActiveCursor(HCURSOR cursor)
    {
        if(cursor)
        {
            previous_cursor_ = ::SetCursor(cursor);
        }
        else if(previous_cursor_)
        {
            ::SetCursor(previous_cursor_);
            previous_cursor_ = NULL;
        }
    }

} //namespace view