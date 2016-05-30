#pragma once
#include <phi.h>
#include "uiApi.h"
#include <core\component.h>
#include <core\geometry.h>
#include <core\color.h>
#include <rendering\font.h>
#include "control.h"

namespace phi
{
    class text :
        public component
    {
    private:
        font* _font;
        control* _control;
        wstring _text;
        color _color;

    private:
        void updateControl();

    public:
        static componentType getComponentType() { return componentType::TEXT; }

    public:
        text(string name);
        ~text();

        wstring getText() const { return _text; }
        font* getFont() const { return  _font; }
        color getColor() const { return _color; }
        control* getControl() const { return _control; }

        UI_API void setText(wstring value);
        UI_API void setFont(font* value);

        void setColor(color value) { _color = value; }
        void setControl(control* value) { _control = value; }
    };
}