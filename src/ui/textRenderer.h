#pragma once
#include <phi.h>
#include <core\component.h>
#include <core\geometry.h>
#include <core\color.h>
#include <rendering\font.h>

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

    public:
        static componentType getComponentType() { return componentType::TEXT_RENDERER; }

    public:
        textRenderer(string name);
        ~textRenderer();

        wstring getText() const { return _text; }
        font* getFont() const { return  _font; }
        color getBackgroundColor() const { return _backgroundColor; }
        color getForegroundColor() const { return _foregroundColor; }

        void setText(wstring value) { _text = value; }
        void setFont(font* value) { _font = value; }
        void setBackgroundColor(color value) { _backgroundColor = value; }
        void setForegroundColor(color value) { _foregroundColor = value; }
    };
}