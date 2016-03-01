#ifndef _PHI_TOOL_TIP_H_
#define _PHI_TOOL_TIP_H_

#include <core\size.h>

#include <rendering\quadRenderer2D.h>
#include <rendering\textRenderer2D.h>

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

namespace phi
{
    class toolTip
    {
    private:
        static const unsigned int TEXT_MARGIN = 6;

        quadRenderer2D* _backgroundRenderer;
        texture* _backgroundTexture;
        textRenderer2D* _foregroundRenderer;
        sizef _viewportSize;
        font* _font;
        std::string _text;
        vec2 _textLocation;
        vec2 _location;
        sizef _size;
        color _textColor;

    public:
        toolTip(sizef viewportSize);
        ~toolTip();

        std::string getText() const { return _text; }
        vec2 getLocation() const { return _location; }
        sizef getSize() const { return _size; }

        void show(const std::string text, const vec2 location, const sizef size);

        void render();
    };
}

#endif