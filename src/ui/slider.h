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
		slider(sizeui viewportSize);
        ~slider();

        color getTrackColor() { return _trackColor; }
        color getSliderColor() { return _sliderColor; }
        eventHandler<eventArgs>* getValueChanged() { return _valueChanged; }
        float getValue() { return _value; }
        float getMinValue() { return _minValue; }
        float getMaxValue() { return _maxValue; }
        float getStepValue() { return _stepValue; }

        void setTrackColor(color value) { _trackColor = value; }
        void setSliderColor(color value);
        void setValue(float value);
        void setMinValue(float value);
        void setMaxValue(float value);
        void setStepValue(float value);

        void setX(int value) override;
        void setY(int value) override;
        void setSize(sizeui value) override;
		void setViewportSize(sizeui value) override;
		void setZIndex(float value) override;

        void onMouseDown(mouseEventArgs* e) override;
        void onMouseUp(mouseEventArgs* e) override;
        void onMouseMove(mouseEventArgs* e) override;
        void onKeyDown(keyboardEventArgs* e) override;
        void onMouseEnter(mouseEventArgs* e) override;
        void onMouseLeave(mouseEventArgs* e) override;

        void onRender() override;
    };
}