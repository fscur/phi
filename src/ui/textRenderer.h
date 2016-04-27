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
        wstring _text;
        font* _font;
        color _foregroundColor;
        color _backgroundColor;
        controlRenderer* _controlRenderer;

    private:
        void updateControlRenderer();

    public:
        static componentType getComponentType() { return componentType::TEXT_RENDERER; }

    public:
        textRenderer(string name);
        ~textRenderer();

        wstring getText() const { return _text; }
        font* getFont() const { return  _font; }
        color getBackgroundColor() const { return _backgroundColor; }
        color getForegroundColor() const { return _foregroundColor; }
        controlRenderer* getControlRenderer() const { return _controlRenderer; }

        UI_API void setText(wstring value);
        UI_API void setFont(font* value);

        void setBackgroundColor(color value) { _backgroundColor = value; }
        void setForegroundColor(color value) { _foregroundColor = value; }
        void setControlRenderer(controlRenderer* value) { _controlRenderer = value; }
    };
}