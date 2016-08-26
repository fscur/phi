#pragma once
#include <phi.h>
#include "uiApi.h"
#include <core\component.h>
#include <core\color.h>
#include <core\image.h>
#include <core\eventHandler.h>

namespace phi
{
    class control :
        public component
    {
    private:
        image* _image;
        eventHandler<vec3>* _onSizeChanged;
        color _color;
        vec3 _size;

    public:
        eventHandler<control*> colorChanged;

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

        void setImage(image* value) { _image = value; }
        UI_API void setColor(color value);
        UI_API void setSize(vec3 value);

        eventHandler<vec3>* getOnSizeChanged() const { return _onSizeChanged; }
    };
}