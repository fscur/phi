#pragma once
#include <phi.h>
#include "uiApi.h"
#include <core/component.h>
#include <core/color.h>
#include <core/image.h>
#include <core/eventHandler.h>

namespace phi
{
    class control :
        public component
    {
    public:
        static componentType getComponentType() { return componentType::CONTROL; }

    public:
        control();
        control(const control& control);
        ~control();

        UI_API component* clone() const override;

        image* getBackgroundImage() const { return _image; }
        color getColor() const { return _color; }
        vec3 getSize() const { return _size; }
        bool isGlassy() const { return _isGlassy; }
        bool isBillboard() const { return _isBillboard; }

        void setImage(image* value) { _image = value; }
        UI_API void setColor(color value);
        UI_API void setSize(vec3 value);
        UI_API void setIsGlassy(bool value) { _isGlassy = value; }
        UI_API void setIsBillboard(bool value) { _isBillboard = value; }

        eventHandler<vec3>* getOnSizeChanged() const { return _onSizeChanged; }

    public:
        eventHandler<control*> colorChanged;

    private:
        image* _image;
        eventHandler<vec3>* _onSizeChanged;
        color _color;
        vec3 _size;
        bool _isGlassy;
        bool _isBillboard;
    };
}