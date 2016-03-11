#pragma once
#include <phi.h>

#include <core\size.h>

#include <rendering\quadRenderer2D.h>
#include <rendering\textRenderer2D.h>

namespace phi
{
    class toolTip
    {
    private:
        static const unsigned int TEXT_MARGIN = 6;

        quadRenderer2D* _backgroundRenderer;
        textRenderer2D* _foregroundRenderer;
        texture* _backgroundTexture;
        sizeui _viewportSize;
        font* _font;
        std::string _text;
        vec2 _textLocation;
        vec2 _location;
        sizeui _size;
        color _textColor;

    public:
        toolTip(sizeui viewportSize);
        ~toolTip();

        std::string getText() const { return _text; }
        vec2 getLocation() const { return _location; }
        sizeui getSize() const { return _size; }

        void show(const std::string text, const vec2 location, const sizeui size);

        void render();
    };
}