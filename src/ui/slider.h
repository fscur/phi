#pragma once
#include <phi.h>

#include "core\color.h"
#include "core\eventHandler.h"
#include "core\eventArgs.h"

#include "rendering\quadRenderer2D.h"

#include "control.h"

namespace phi
{
    class slider : public control
    {
    private:
        //static const unsigned int TRACK_HEIGHT = 10;
        //static const unsigned int SLIDER_HEIGHT = 8;

        quadRenderer2D* _trackQuad;
        quadRenderer2D* _sliderQuad;
        texture* _texture;
        bool _isDragginTrack;

        color _trackColor;
        color _sliderColor;
        color _currentSliderColor;
        eventHandler<eventArgs>* _valueChanged;
        float _value;
        float _minValue;
        float _maxValue;
        float _stepValue;

        void updateSlider();
		void changeValue(vec2 pos);

    public:
		UI_API slider(sizef viewportSize);
        UI_API ~slider();

        UI_API color getTrackColor() { return _trackColor; }
        UI_API color getSliderColor() { return _sliderColor; }
        UI_API eventHandler<eventArgs>* getValueChanged() { return _valueChanged; }
        UI_API float getValue() { return _value; }
        UI_API float getMinValue() { return _minValue; }
        UI_API float getMaxValue() { return _maxValue; }
        UI_API float getStepValue() { return _stepValue; }

        UI_API void setTrackColor(color value) { _trackColor = value; }
        UI_API void setSliderColor(color value);
        UI_API void setValue(float value);
        UI_API void setMinValue(float value);
        UI_API void setMaxValue(float value);
        UI_API void setStepValue(float value);

        UI_API void setX(int value) override;
        UI_API void setY(int value) override;
        UI_API void setSize(sizef value) override;
		UI_API void setViewportSize(sizef value) override;
		UI_API void setZIndex(float value) override;

        UI_API void onMouseDown(mouseEventArgs* e) override;
        UI_API void onMouseUp(mouseEventArgs* e) override;
        UI_API void onMouseMove(mouseEventArgs* e) override;
        UI_API void onKeyDown(keyboardEventArgs e) override;
        UI_API void onMouseEnter(mouseEventArgs* e) override;
        UI_API void onMouseLeave(mouseEventArgs* e) override;

        UI_API void onRender() override;
    };
}