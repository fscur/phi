#pragma once
#include <phi.h>
#include "uiApi.h"
#include <core\component.h>
#include <core\geometry.h>
#include <core\color.h>
#include <rendering\font.h>
#include "controlRenderer.h"

namespace phi
{
    class textRenderer :
        public component
    {
    private:
        font* _font;
        controlRenderer* _controlRenderer;
        wstring _text;
        color _color;

    private:
        void updateControlRenderer();

    public:
        static componentType getComponentType() { return componentType::TEXT_RENDERER; }

    public:
        textRenderer(string name);
        ~textRenderer();

        wstring getText() const { return _text; }
        font* getFont() const { return  _font; }
        color getColor() const { return _color; }
        controlRenderer* getControlRenderer() const { return _controlRenderer; }

        UI_API void setText(wstring value);
        UI_API void setFont(font* value);

        void setColor(color value) { _color = value; }
        void setControlRenderer(controlRenderer* value) { _controlRenderer = value; }
    };
}