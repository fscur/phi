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
        control* _controlRenderer;
        wstring _text;
        color _color;

    private:
        void updateControlRenderer();

    public:
        static componentType getComponentType() { return componentType::TEXT_RENDERER; }

    public:
        text(string name);
        ~text();

        wstring getText() const { return _text; }
        font* getFont() const { return  _font; }
        color getColor() const { return _color; }
        control* getControlRenderer() const { return _controlRenderer; }

        UI_API void setText(wstring value);
        UI_API void setFont(font* value);

        void setColor(color value) { _color = value; }
        void setControlRenderer(control* value) { _controlRenderer = value; }
    };
}