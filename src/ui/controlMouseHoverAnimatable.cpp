#include <precompiled.h>

#include "controlMouseHoverAnimatable.h"

namespace phi
{
    controlMouseHoverAnimatable::controlMouseHoverAnimatable(control* control, animator* animator) :
        _control(control),
        _animator(animator),
        _hoverColor(color::orange)
    {
        assert(control);
        assert(animator);

        createMouseEnterAnimation();
        createMouseLeaveAnimation();
    }

    controlMouseHoverAnimatable::controlMouseHoverAnimatable(const controlMouseHoverAnimatable& original) :
        _control(original._control),
        _animator(original._animator),
        _mouseEnterAnimation(original._mouseEnterAnimation),
        _mouseLeaveAnimation(original._mouseLeaveAnimation)
    {
    }

    controlMouseHoverAnimatable::~controlMouseHoverAnimatable()
    {
    }

    void controlMouseHoverAnimatable::createMouseEnterAnimation()
    {
        _mouseEnterAnimation = colorAnimation([&](color color) { _control->setColor(color); });
        _animator->addAnimation(&_mouseEnterAnimation);
    }

    void controlMouseHoverAnimatable::createMouseLeaveAnimation()
    {
        _mouseLeaveAnimation = colorAnimation([&](color color) { _control->setColor(color); });
        _animator->addAnimation(&_mouseLeaveAnimation);
    }

    void controlMouseHoverAnimatable::onMouseEnter()
    {
        if (_mouseLeaveAnimation.isRunning())
        {
            _mouseLeaveAnimation.stop();
            _mouseEnterAnimation.start(_control->getColor(), _hoverColor, 0.3f);
        }
        else
        {
            _mouseEnterBegining = _control->getColor();
            _mouseEnterAnimation.start(_mouseEnterBegining, _hoverColor, 0.3f);
        }
    }

    void controlMouseHoverAnimatable::onMouseLeave()
    {
        if (_mouseEnterAnimation.isRunning())
        {
            _mouseEnterAnimation.stop();
        }

        _mouseLeaveAnimation.start(_control->getColor(), _mouseEnterBegining, 0.3f);
    }

    iMouseHoverAnimatable* controlMouseHoverAnimatable::clone()
    {
        return new controlMouseHoverAnimatable(*this);
    }
}