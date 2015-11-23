#ifndef _PHI_TOOL_TIP_H_
#define _PHI_TOOL_TIP_H_

#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif
#include "phi/core/size.h"
#include "phi/rendering/quadRenderer2D.h"
#include "phi/rendering/textRenderer2D.h"

namespace phi
{
    class toolTip
    {
    private:
        static const unsigned int TEXT_MARGIN = 6;

        quadRenderer2D* _backgroundRenderer;
        texture* _backgroundTexture;
        textRenderer2D* _foregroundRenderer;
        size<GLuint> _viewportSize;
        font* _font;
        std::string _text;
        glm::vec2 _textLocation;
        glm::vec2 _location;
        size<GLuint> _size;
        color _textColor;

    public:
        toolTip(size<GLuint> viewportSize);
        ~toolTip();

        std::string getText() const { return _text; }
        glm::vec2 getLocation() const { return _location; }
        size<GLuint> getSize() const { return _size; }

        void show(const std::string text, const glm::vec2 location, const size<GLuint> size);

        void render();
    };
}

#endif