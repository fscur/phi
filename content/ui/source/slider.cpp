#include "slider.h"
#include <glm\glm.hpp>
#include "colorAnimator.h"

namespace phi
{
    slider::slider(size<GLuint> viewportSize) : control(viewportSize)
    {
        _texture = uiRepository::repository->getResource<texture>("button");
        _trackQuad = new quadRenderer2D(glm::vec2(0, 0), _zIndex, size<GLuint>(0, 0, 0), viewportSize);
        _sliderQuad = new quadRenderer2D(glm::vec2(0, 0), _zIndex + 0.01f, size<GLuint>(0, 0, 0), viewportSize);
        _valueChanged = new eventHandler<eventArgs>();
        _value = 0;
        _minValue = 0;
        _maxValue = 1;
        _stepValue = 1;
        _isDragginTrack = false;
    }

    slider::~slider()
    {
        DELETE(_sliderQuad);
        DELETE(_trackQuad);
    }

    void slider::setX(int value)
    {
        _x = value;
        _trackQuad->setLocation(glm::vec2(_x, _trackQuad->getLocation().y));
        _trackQuad->update();
        updateSlider();
    }

    void slider::setY(int value)
    {
        _y = value;
        _trackQuad->setLocation(glm::vec2(_trackQuad->getLocation().x, _y));
        _trackQuad->update();
        updateSlider();
    }

    void slider::setSize(size<GLuint> value)
    {
        _size = value;
        _trackQuad->setLocation(glm::vec2(_trackQuad->getLocation().x, _y));
        _trackQuad->setSize(size<GLuint>(_size.width, _size.height));
        _trackQuad->update();
        updateSlider();
    }

    void slider::setViewportSize(size<GLuint> value)
    {
        control::setViewportSize(value);
        _trackQuad->setViewportSize(getViewportSize());
        _trackQuad->update();
        _sliderQuad->setViewportSize(getViewportSize());
        _sliderQuad->update();
    }

    void slider::setZIndex(float value)
    {
        _zIndex = value;
        _trackQuad->setZIndex(_zIndex);
        _sliderQuad->setZIndex(_zIndex + 0.01f);
    }

    void slider::setValue(float value)
    {
        if (value < _minValue)
            throw "Value should be greater than the minimum value.";
        if (value > _maxValue)
            throw "Value should be smaller than the maximum value.";

        _value = value;

        updateSlider();

        if (_valueChanged->isBound())
            _valueChanged->invoke(eventArgs());
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

    void slider::updateSlider()
    {
        float percent = (_value - _minValue) / (_maxValue - _minValue);

        float y = _y + 1;
        _sliderQuad->setLocation(glm::vec2(_trackQuad->getLocation().x, y));
        _sliderQuad->setSize(size<GLuint>((unsigned int)(percent * _size.width), _size.height - 2));
        _sliderQuad->update();
    }

    void slider::changeValue(glm::vec2 screenPos)
    {
        float totalValue = _maxValue - _minValue;
        float totalSteps = totalValue / _stepValue;

        float diffDistance = (float)(screenPos.x - _x);
        float percent = diffDistance / (float)_size.width;

        int steps = (int)glm::round(totalSteps * percent);
        steps = glm::max(glm::min(steps, (int)totalSteps), 0);
        setValue(_minValue + steps * _stepValue);
    }

    void slider::render()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _trackQuad->render(_texture, _trackColor);
        _sliderQuad->render(_texture, _currentSliderColor);
        glDisable(GL_BLEND);
    }

    void slider::onMouseDown(mouseEventArgs e)
    {
        if (getIsMouseOver() && e.leftButtonPressed && !_isDragginTrack)
        {
            _isDragginTrack = true;
            changeValue(glm::vec2(e.x, e.y));
        }
    }

    void slider::onMouseUp(mouseEventArgs e)
    {
        if (e.leftButtonPressed && _isDragginTrack)
            _isDragginTrack = false;
    }

    void slider::onMouseMove(mouseEventArgs e)
    {
        if (_isDragginTrack)
            changeValue(glm::vec2(e.x, e.y));
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

    void slider::onMouseEnter(mouseEventArgs e)
    {
        colorAnimator::animateColor(&_currentSliderColor, color(glm::min(1.0f, _sliderColor.r + 0.3f), glm::min(1.0f, _sliderColor.g + 0.3f), glm::min(1.0f, _sliderColor.b + 0.3f), 1), 500);
    }

    void slider::onMouseLeave(mouseEventArgs e)
    {
        colorAnimator::animateColor(&_currentSliderColor, _sliderColor, 500);
    }
}