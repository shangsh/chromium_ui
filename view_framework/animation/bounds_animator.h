
#ifndef __bounds_animator_h__
#define __bounds_animator_h__

#pragma once

#include <map>

#include "gfx/rect.h"

#include "animation/animation.h"
#include "animation/animation_container.h"

class SlideAnimation;

namespace view
{

    class BoundsAnimator;
    class View;

    class BoundsAnimatorObserver
    {
    public:
        // Invoked when all animations are complete.
        virtual void OnBoundsAnimatorDone(BoundsAnimator* animator) = 0;
    };

    // Bounds animator is responsible for animating the bounds of a view from the
    // the views current location and size to a target position and size. To use
    // BoundsAnimator invoke AnimateViewTo for the set of views you want to
    // animate.
    //
    // BoundsAnimator internally creates an animation for each view. If you need
    // a specific animation invoke SetAnimationForView after invoking AnimateViewTo.
    // You can attach an AnimationDelegate to the individual animation for a view
    // by way of SetAnimationDelegate. Additionally you can attach an observer to
    // the BoundsAnimator that is notified when all animations are complete.
    class BoundsAnimator : public AnimationDelegate,
        public AnimationContainer::Observer
    {
    public:
        // If |delete_when_done| is set to true in |SetAnimationDelegate| the
        // |AnimationDelegate| must subclass this class.
        class OwnedAnimationDelegate : public AnimationDelegate
        {
        public:
            virtual ~OwnedAnimationDelegate() {}
        };

        explicit BoundsAnimator(View* parent);
        ~BoundsAnimator();

        // Starts animating |view| from its current bounds to |target|. If there is
        // already an animation running for the view it's stopped and a new one
        // started. If an AnimationDelegate has been set for |view| it is removed
        // (after being notified that the animation was canceled).
        void AnimateViewTo(View* view, const gfx::Rect& target);

        // Sets the animation for the specified view. BoundsAnimator takes ownership
        // of the specified animation.
        void SetAnimationForView(View* view, SlideAnimation* animation);

        // Returns the animation for the specified view. BoundsAnimator owns the
        // returned Animation.
        const SlideAnimation* GetAnimationForView(View* view);

        // Stops animating the specified view. If the view was scheduled for deletion
        // it is deleted. This does nothing if |view| is not currently animating.
        void StopAnimatingView(View* view);

        // Sets the delegate for the animation created for the specified view. If
        // |delete_when_done| is true the |delegate| is deleted when done and
        // |delegate| must subclass OwnedAnimationDelegate.
        void SetAnimationDelegate(View* view, AnimationDelegate* delegate,
            bool delete_when_done);

        // Returns true if BoundsAnimator is animating the bounds of |view|.
        bool IsAnimating(View* view) const;

        // Returns true if BoundsAnimator is animating any view.
        bool IsAnimating() const;

        // Cancels all animations, leaving the views at their current location and
        // size. Any views marked for deletion are deleted.
        void Cancel();

        void set_observer(BoundsAnimatorObserver* observer)
        {
            observer_ = observer;
        }

    protected:
        // Creates the animation to use for animating views.
        virtual SlideAnimation* CreateAnimation();

    private:
        // Tracks data about the view being animated.
        struct Data
        {
            Data() : delete_delegate_when_done(false),
                animation(NULL), delegate(NULL) {}

            // If true the delegate is deleted when done.
            bool delete_delegate_when_done;

            // The initial bounds.
            gfx::Rect start_bounds;

            // Target bounds.
            gfx::Rect target_bounds;

            // The animation. We own this.
            SlideAnimation* animation;

            // Additional delegate for the animation, may be null.
            AnimationDelegate* delegate;
        };

        // Used by AnimationEndedOrCanceled.
        enum AnimationEndType
        {
            ANIMATION_ENDED,
            ANIMATION_CANCELED
        };

        typedef std::map<View*, Data> ViewToDataMap;

        typedef std::map<const Animation*, View*> AnimationToViewMap;

        // Removes references to |view| and its animation. This does NOT delete the
        // animation or delegate.
        void RemoveFromMaps(View* view);

        // Does the necessary cleanup for |data|. If |send_cancel| is true and a
        // delegate has been installed on |data| AnimationCanceled is invoked on it.
        void CleanupData(bool send_cancel, Data* data, View* view);

        // Used when changing the animation for a view. This resets the maps for
        // the animation used by view and returns the current animation. Ownership
        // of the returned animation passes to the caller.
        Animation* ResetAnimationForView(View* view);

        // Invoked from AnimationEnded and AnimationCanceled.
        void AnimationEndedOrCanceled(const Animation* animation,
            AnimationEndType type);

        // AnimationDelegate overrides.
        virtual void AnimationProgressed(const Animation* animation);
        virtual void AnimationEnded(const Animation* animation);
        virtual void AnimationCanceled(const Animation* animation);

        // AnimationContainer::Observer overrides.
        virtual void AnimationContainerProgressed(AnimationContainer* container);
        virtual void AnimationContainerEmpty(AnimationContainer* container);

        // Parent of all views being animated.
        View* parent_;

        BoundsAnimatorObserver* observer_;

        // All animations we create up with the same container.
        scoped_refptr<AnimationContainer> container_;

        // Maps from view being animated to info about the view.
        ViewToDataMap data_;

        // Makes from animation to view.
        AnimationToViewMap animation_to_view_;

        // As the animations we created update (AnimationProgressed is invoked) this
        // is updated. When all the animations have completed for a given tick of
        // the timer (AnimationContainerProgressed is invoked) the parent_ is asked
        // to repaint these bounds.
        gfx::Rect repaint_bounds_;

        DISALLOW_COPY_AND_ASSIGN(BoundsAnimator);
    };

} //namespace view

#endif //__bounds_animator_h__