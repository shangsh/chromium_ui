
#ifndef __single_split_view_h__
#define __single_split_view_h__

#pragma once

#include "../view.h"

namespace view
{

    // SingleSplitView lays out two views horizontally. A splitter exists between
    // the two views that the user can drag around to resize the views.
    class SingleSplitView : public View
    {
    public:
        enum Orientation
        {
            HORIZONTAL_SPLIT,
            VERTICAL_SPLIT
        };

        SingleSplitView(View* leading, View* trailing, Orientation orientation);

        virtual void DidChangeBounds(const gfx::Rect& previous,
            const gfx::Rect& current);

        virtual void Layout();

        virtual AccessibilityTypes::Role GetAccessibleRole();

        // SingleSplitView's preferred size is the sum of the preferred widths
        // and the max of the heights.
        virtual gfx::Size GetPreferredSize();

        // Overriden to return a resize cursor when over the divider.
        virtual HCURSOR GetCursorForPoint(Event::EventType event_type,
            const gfx::Point& p);

        void set_divider_offset(int divider_offset)
        {
            divider_offset_ = divider_offset;
        }
        int divider_offset() { return divider_offset_; }

        // Sets whether the leading component is resized when the split views size
        // changes. The default is true. A value of false results in the trailing
        // component resizing on a bounds change.
        void set_resize_leading_on_bounds_change(bool resize)
        {
            resize_leading_on_bounds_change_ = resize;
        }

    protected:
        virtual bool OnMousePressed(const MouseEvent& event);
        virtual bool OnMouseDragged(const MouseEvent& event);
        virtual void OnMouseReleased(const MouseEvent& event, bool canceled);

    private:
        // Returns true if |x| or |y| is over the divider.
        bool IsPointInDivider(const gfx::Point& p);

        // Returns width in case of horizontal split and height otherwise.
        int GetPrimaryAxisSize()
        {
            return GetPrimaryAxisSize(width(), height());
        }

        int GetPrimaryAxisSize(int h, int v)
        {
            return is_horizontal_ ? h : v;
        }

        // Used to track drag info.
        struct DragInfo
        {
            // The initial coordinate of the mouse when the user started the drag.
            int initial_mouse_offset;
            // The initial position of the divider when the user started the drag.
            int initial_divider_offset;
        };

        DragInfo drag_info_;

        // Orientation of the split view.
        bool is_horizontal_;

        // Position of the divider.
        int divider_offset_;

        bool resize_leading_on_bounds_change_;

        DISALLOW_COPY_AND_ASSIGN(SingleSplitView);
    };

} //namespace view

#endif //__single_split_view_h__