#include "pickingFrameBuffer.h"

namespace phi
{
    pickingFrameBuffer::pickingFrameBuffer(size<GLuint> size) : frameBuffer("picking", size, color::transparent)
    {
    }

    pickingFrameBuffer::~pickingFrameBuffer()
    {
    }

    void pickingFrameBuffer::init()
    {
        frameBuffer::init();

        texture* t = texture::create(_size, GL_RGBA);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        _pickingTexture = t;

        renderTarget* r = newRenderTarget("selected", t);

        addRenderTarget(r);
    }

    void pickingFrameBuffer::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    GLuint pickingFrameBuffer::pick(glm::vec2 mousePos)
    {
        bindForReading();

        std::vector<unsigned char> pixels( 4 );

        glReadPixels(mousePos.x, _size.height - mousePos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

        GLuint r = (GLuint)pixels[0];
        GLuint g = (GLuint)pixels[1];
        GLuint b = (GLuint)pixels[2];

        b = b << 16;
        g = g << 8;

        GLuint id = r | g | b;




        return id;
    }
}