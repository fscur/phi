#include <precompiled.h>
#include "slider.h"

#include "colorAnimator.h"

namespace phi
{
    slider::slider(sizeui viewportSize) : control(viewportSize)
    {
        _texture = uiRepository::textureButton;
        auto size = sizeui(viewportSize.w, viewportSize.h, viewportSize.d);
        _trackQuad = new quadRenderer2D(vec2(0, 0), _zIndex, sizeui(0, 0, 0), size);
        _sliderQuad = new quadRenderer2D(vec2(0, 0), _zIndex + 0.01f, sizeui(0, 0, 0), size);
        _valueChanged = new eventHandler<eventArgs>();
        _value = 0;
        _minValue = 0;
        _maxValue = 1;
        _stepValue = 1;
        _isDragginTrack = false;
    }

    slider::~slider()
    {
        safeDelete(_sliderQuad);
        safeDelete(_trackQuad);
    }

    void slider::setX(int value)
    {
        _x = value;
        _trackQuad->setLocation(vec2(_x, _trackQuad->getLocation().y));
        _trackQuad->update();
        updateSlider();
    }

    void slider::setY(int value)
    {
        _y = value;
        _trackQuad->setLocation(vec2(_trackQuad->getLocation().x, _y));
        _trackQuad->update();
        updateSlider();
    }

    void slider::setSize(sizeui value)
    {
        _size = value;
        _trackQuad->setLocation(vec2(_trackQuad->getLocation().x, _y));
        _trackQuad->setSize(sizeui(_size.w, _size.h));
        _trackQuad->update();
        updateSlider();
    }

    void slider::setViewportSize(sizeui value)
    {
        control::setViewportSize(value);
        auto size = sizeui(value.w, value.h, value.d);
        _trackQuad->setViewportSize(size);
        _trackQuad->update();
        _sliderQuad->setViewportSize(size);
        _sliderQuad->update();
    }

    void slider::setZIndex(float value)
    {
        _zIndex = value;
        _trackQuad->setZIndex(_zIndex);
        _trackQuad->update();
        _sliderQuad->setZIndex(_zIndex + 0.01f);
        _sliderQuad->update();
    }

    void slider::setValue(float value)
    {
        if (value < _minValue)
            throw "Value should be greater than the minimum value.";
        if (value > _maxValue)
            throw "Value should be smaller than the maximum value.";

        _value = value;

        updateSlider();

        _valueChanged->raise(eventArgs());
    }

    void slider::setMinValue(float value)
    {
        if (value > _maxValue)
            throw "Minimum value should be smaller than or equal to the maximum value.";
        if (_maxValue - value < _stepValue)
            throw "The difference between the maximum value and the minimum value should be greater than or equal to the step value.";

        _minValue = value; 
        if (_value < _minValue)
            setValue(_minValue);

        updateSlider();
    }

    void slider::setMaxValue(float value)
    {
        if (value < _minValue)
            throw "Maximum value should be greather than or equal to the minimum value.";
        if (value - _minValue < _stepValue)
            throw "The difference between the maximum value and the minimum value should be greater than or equal to the step value.";

        _maxValue = value; 
        if (_value > _maxValue)
            setValue(_maxValue);

        updateSlider();
    }

    void slider::setStepValue(float value) 
    { 
        if (_maxValue - _minValue < value)
            throw "The difference between the maximum value and the minimum value should be greater than or equal to the step value.";

        _stepValue = value;

        updateSlider();
    }

    void slider::setSliderColor(color value)
    {
        _sliderColor = value;
        _currentSliderColor = _sliderColor;
    }

    void slider::updateSlider()
    {
        auto percent = (_value - _minValue) / (_maxValue - _minValue);

        auto y = _y + 1;
        _sliderQuad->setLocation(vec2(_trackQuad->getLocation().x, (float)y));
        _sliderQuad->setSize(sizeui((unsigned int)(percent * _size.w), _size.h - 2));
        _sliderQuad->update();
    }

    void slider::changeValue(vec2 screenPos)
    {
        float totalValue = _maxValue - _minValue;
        float totalSteps = totalValue / _stepValue;

        float diffDistance = (float)(screenPos.x - _x);
        float percent = diffDistance / (float)_size.w;

        int steps = (int)round(totalSteps * percent);
        steps = glm::max(glm::min(steps, (int)totalSteps), 0);
        setValue(_minValue + steps * _stepValue);
    }

    void slider::onRender()
    {
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _trackQuad->render(_texture, _trackColor);
        _sliderQuad->render(_texture, _currentSliderColor);
        //glDisable(GL_BLEND);
    }

    void slider::onMouseDown(mouseEventArgs* e)
    {
        if (getIsMouseOver() && e->leftButtonPressed && !_isDragginTrack)
        {
            _isDragginTrack = true;
            changeValue(vec2(e->x, e->y));
            e->handled = true;
        }
    }

    void slider::onMouseUp(mouseEventArgs* e)
    {
        if (e->leftButtonPressed && _isDragginTrack)
        {
            _isDragginTrack = false;
            e->handled = true;
        }
    }

    void slider::onMouseMove(mouseEventArgs* e)
    {
        if (_isDragginTrack)
            changeValue(vec2(e->x, e->y));
    }

    void slider::onKeyDown(keyboardEventArgs e)
    {
        switch (e.key)
        {
        case PHIK_LEFT:
            setValue(glm::max(_minValue, _value - _stepValue));
            break;
        case PHIK_RIGHT:
            setValue(glm::min(_maxValue, _value + _stepValue));
            break;
        }
    }

    void slider::onMouseEnter(mouseEventArgs* e)
    {
        colorAnimator::animateColor(&_currentSliderColor, color(glm::min(1.0f, _sliderColor.r + 0.3f), glm::min(1.0f, _sliderColor.g + 0.3f), glm::min(1.0f, _sliderColor.b + 0.3f), 1), 500);
    }

    void slider::onMouseLeave(mouseEventArgs* e)
    {
        colorAnimator::animateColor(&_currentSliderColor, _sliderColor, 500);
    }
}