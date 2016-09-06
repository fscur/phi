#pragma once
#include <phi.h>

#include <core/transform.h>

#include <animation/animator.h>
#include <animation/iMouseClickAnimatable.h>
#include <animation/translateAnimation.h>

#include "control.h"

namespace phi
{
    class controlMouseClickAnimatable :
        public iMouseClickAnimatable
    {
    private:
        const float MOUSE_DOWN_DURATION = 0.1f;
        const float MOUSE_UP_DURATION = 0.1f;
        const float TRANSLATION_FACTOR = 5.0f;

        animator* _animator;
        control* _control;

        translateAnimation* _mouseDownAnimation;
        translateAnimation* _mouseUpAnimation;

        vec3 _lastMouseDownBegining;
        vec3 _lastMouseDownDestination;

    public:
        controlMouseClickAnimatable(control* control, animator* animator);
        controlMouseClickAnimatable(const controlMouseClickAnimatable& original);
        ~controlMouseClickAnimatable();

        virtual void onMouseDown() override;
        virtual void onMouseUp() override;

    private:
        vec3 getControlNormal();
        void startPartialMouseDownAnimation();
        void startFullMouseDownAnimation();
    };
}