#include "toggleButton.h"
#include "colorAnimator.h"

namespace phi
{
    toggleButton::toggleButton(size<GLuint> viewportSize) : button(viewportSize)
    {
        _checked = false;
        _checkedColor = color::white;
        _checkedCurrentColor = color::fromRGBA(0.0f, 0.0f, 0.0f, 0.0f);
        _checkedChanging = new eventHandler<controlCancelEventArgs*>();
        _checkedChanged = new eventHandler<controlEventArgs>();
        _checkedBackgroundRenderer = new quadRenderer2D(glm::vec2(0, 0), _zIndex + 0.001f, size<GLuint>(0, 0, 0), viewportSize);
    }

    void toggleButton::setX(int value)
    {
        button::setX(value);
        _checkedBackgroundRenderer->setLocation(glm::vec2(_x, _y));
        _checkedBackgroundRenderer->update();
    }

    void toggleButton::setY(int value)
    {
        button::setY(value);
        _checkedBackgroundRenderer->setLocation(glm::vec2(_x, _y));
        _checkedBackgroundRenderer->update();
    }

    void toggleButton::setZIndex(float value)
    {
        button::setZIndex(value);
        _checkedBackgroundRenderer->setZIndex(_zIndex + 0.001f);
        _checkedBackgroundRenderer->update();
    }

    void toggleButton::setSize(size<GLuint> value)
    {
        button::setSize(value);
        _checkedBackgroundRenderer->setSize(value);
        _checkedBackgroundRenderer->update();
    }

    void toggleButton::setViewportSize(size<GLuint> value)
    {
        button::setViewportSize(value);
        _checkedBackgroundRenderer->setViewportSize(getViewportSize());
        _checkedBackgroundRenderer->update();
    }

    void toggleButton::setChecked(const bool value)
    {
        if (_checkedChanging->isBound())
        {
            auto cancelArgs = new controlCancelEventArgs(this);
            _checkedChanging->invoke(cancelArgs);
            if (cancelArgs->cancel)
                return;
        }

        _checked = value;

        if (_checkedChanged->isBound())
            _checkedChanged->invoke(controlEventArgs(this));

        if (_checked)
        {
            colorAnimator::animateColor(&_currentBackgroundColor, _checkedColor, 300);
            //colorAnimator::animateColor(&_checkedCurrentColor, _checkedColor, 300);
            //colorAnimator::animateColor(&_currentBackgroundColor, color::fromRGBA(_backgroundColor.r, _backgroundColor.g, _backgroundColor.b, 0.0f), 300);
        }
        else
        {
            colorAnimator::animateColor(&_checkedCurrentColor, color(_checkedColor.r, _checkedColor.g, _checkedColor.b, 0.0f), 300);
            colorAnimator::animateColor(&_currentBackgroundColor, _backgroundColor, 300);
        }
    }

    void toggleButton::onClick()
    {
        setChecked(!_checked);
    }

    void toggleButton::onRender()
    {
        control::controlsScissors->pushScissor(_x, _y, _size.width, _size.height);
        control::controlsScissors->enable();

        //glPushAttrib(GL_SCISSOR_BIT);
        //glEnable(GL_SCISSOR_TEST);
        //glScissor(_x, (_viewportSize.height - _size.height - _y), _size.width, _size.height);
        renderBackground();
        //_checkedBackgroundRenderer->render(_backgroundTexture, _checkedCurrentColor);
        renderImage();
        renderOverlay();
        renderForeground();
        //glPopAttrib();

        control::controlsScissors->popScissor();
        control::controlsScissors->disable();
    }
}