
#ifndef __slide_animation_h__
#define __slide_animation_h__

#pragma once

#include "linear_animation.h"
#include "tween.h"

// Slide Animation
//
// ���ڸ���ʵ�ֿ��涯��. �����÷�:
//
//     #include "app/slide_animation.h"
//
//     class MyClass : public AnimationDelegate {
//     public:
//       MyClass() {
//         animation_.reset(new SlideAnimation(this));
//         animation_->SetSlideDuration(500);
//       }
//       void OnMouseOver() {
//         animation_->Show();
//       }
//       void OnMouseOut() {
//         animation_->Hide();
//       }
//       void AnimationProgressed(const Animation* animation) {
//         if(animation == animation_.get()) {
//           Layout();
//           SchedulePaint();
//         } else if(animation == other_animation_.get()) {
//           ...
//         }
//       }
//       void Layout() {
//         if(animation_->is_animating()) {
//           hover_image_.SetOpacity(animation_->GetCurrentValue());
//         }
//       }
//     private:
//       scoped_ptr<SlideAnimation> animation_;
//     }
class SlideAnimation : public LinearAnimation
{
public:
    explicit SlideAnimation(AnimationDelegate* target);
    virtual ~SlideAnimation();

    // ���ö���״̬��0.
    virtual void Reset();
    virtual void Reset(double value);

    // ��ʼ��ʾ���������������صĶ��������.
    virtual void Show();

    // ��ʼ���ض�������������ʾ�Ķ��������.
    virtual void Hide();

    // ���û���������ʱ��. ע�������������ִ��, ���õ�ֵ����������Ч.
    virtual void SetSlideDuration(int duration) { slide_duration_ = duration; }
    int GetSlideDuration() const { return slide_duration_; }
    void SetTweenType(Tween::Type tween_type) { tween_type_ = tween_type; }

    double GetCurrentValue() const { return value_current_; }
    bool IsShowing() const { return showing_; }
    bool IsClosing() const { return !showing_ && value_end_<value_current_; }

private:
    void AnimateToState(double state);

    AnimationDelegate* target_;

    Tween::Type tween_type_;

    // ����ȷ������ִ�з�ʽ.
    bool showing_;

    // ����ֵ. ����Animation::state_�ṩ�����Ŀ�����.
    double value_start_;
    double value_end_;
    double value_current_;

    // in/out����������ʱ��. ȱʡ��kHoverFadeDurationMS, ����ͨ��SetDuration
    // ������ֵ.
    int slide_duration_;

    DISALLOW_COPY_AND_ASSIGN(SlideAnimation);
};

#endif //__slide_animation_h__