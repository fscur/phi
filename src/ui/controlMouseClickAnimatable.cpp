#include <precompiled.h>

#include <core/node.h>

#include "controlMouseClickAnimatable.h"

namespace phi
{
    controlMouseClickAnimatable::controlMouseClickAnimatable(control* control, animator* animator) :
        _control(control),
        _animator(animator)
    {
        assert(control);
        assert(animator);

        auto updateNodePosition = [&](vec3 value)
        {
            _control->getNode()->setPosition(value);
        };

        _mouseDownAnimation = new translateAnimation(updateNodePosition);
        _mouseUpAnimation = new translateAnimation(updateNodePosition);

        _animator->addAnimation(_mouseDownAnimation);
        _animator->addAnimation(_mouseUpAnimation);
    }

    controlMouseClickAnimatable::controlMouseClickAnimatable(const controlMouseClickAnimatable & original) :
        _control(original._control),
        _animator(original._animator),
        _mouseDownAnimation(original._mouseDownAnimation),
        _mouseUpAnimation(original._mouseUpAnimation)
    {
    }

    controlMouseClickAnimatable::~controlMouseClickAnimatable()
    {
        safeDelete(_mouseDownAnimation);
        safeDelete(_mouseUpAnimation);
    }

    void controlMouseClickAnimatable::onMouseDown()
    {
        if (_mouseUpAnimation->isRunning())
        {
            _mouseUpAnimation->stop();
            startPartialMouseDownAnimation();
        }
        else
        {
            startFullMouseDownAnimation();
        }
    }

    void controlMouseClickAnimatable::onMouseUp()
    {
        if (_mouseDownAnimation->isRunning())
        {
            _mouseDownAnimation->stop();
        }

        auto currentPosition = _control->getNode()->getTransform()->getLocalPosition();
        _mouseUpAnimation->start(currentPosition, _lastMouseDownBegining, MOUSE_UP_DURATION);
    }

    iMouseClickAnimatable* controlMouseClickAnimatable::clone()
    {
        return new controlMouseClickAnimatable(*this);
    }

    void controlMouseClickAnimatable::startPartialMouseDownAnimation()
    {
        auto currentPostion = _control->getNode()->getTransform()->getLocalPosition();
        _mouseDownAnimation->start(currentPostion, _lastMouseDownDestination, MOUSE_DOWN_DURATION);
    }

    void controlMouseClickAnimatable::startFullMouseDownAnimation()
    {
        auto normal = getControlNormal();
        auto displacement = normal * TRANSLATION_FACTOR * (-1.f);

        _lastMouseDownBegining = _control->getNode()->getTransform()->getLocalPosition();
        _lastMouseDownDestination = _lastMouseDownBegining + displacement;

        _mouseDownAnimation->start(_lastMouseDownBegining, _lastMouseDownDestination, MOUSE_DOWN_DURATION);
    }

    vec3 controlMouseClickAnimatable::getControlNormal()
    {
        auto normal = vec4(0.0f, 0.0f, 1.0f, 0.0f);
        auto transform = _control->getNode()->getTransform();
        auto transformedNormal = transform->getModelMatrix() * normal;

        return vec3(transformedNormal.x, transformedNormal.y, transformedNormal.z);
    }
}