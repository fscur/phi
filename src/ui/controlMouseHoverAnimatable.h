#pragma once
#include <phi.h>

#include <animation/iMouseHoverAnimatable.h>
#include <animation/animator.h>
#include <animation/colorAnimation.h>

#include "control.h"

namespace phi
{
    class controlMouseHoverAnimatable :
        public iMouseHoverAnimatable
    {
    private:
        animator* _animator;
        control* _control;

        colorAnimation _mouseEnterAnimation;
        colorAnimation _mouseLeaveAnimation;

        color _mouseEnterBegining;
        color _mouseLeaveDestination;

        color _hoverColor;

    public:
        controlMouseHoverAnimatable(control* control, animator* animator);
        controlMouseHoverAnimatable(const controlMouseHoverAnimatable& original);
        ~controlMouseHoverAnimatable();

        virtual void onMouseEnter() override;
        virtual void onMouseLeave() override;

        void setHoverColor(color color) { _hoverColor = color; }

    private:
        void createMouseEnterAnimation();
        void createMouseLeaveAnimation();
    };
}