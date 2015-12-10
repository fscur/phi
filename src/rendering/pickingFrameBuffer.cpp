#include <phi/rendering/pickingFrameBuffer.h>

namespace phi
{
    pickingFrameBuffer::pickingFrameBuffer(sizef size) : frameBuffer("picking", size, color::transparent)
    {
    }

    pickingFrameBuffer::~pickingFrameBuffer()
    {
    }

    void pickingFrameBuffer::init()
    {
        frameBuffer::init();

        bind();

        texture* t = texture::create((uint)_size.w, (uint)_size.h, GL_RGBA);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        _pickingTexture = t;

        renderTarget* r = newRenderTarget("selected", t);

        addRenderTarget(r);

        unbind();
    }

    void pickingFrameBuffer::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    GLuint pickingFrameBuffer::pick(vec2 mousePos)
    {
        bindForReading();

        std::vector<unsigned char> pixels(4);

        glReadPixels((GLint)mousePos.x, (GLint)(_size.h - mousePos.y), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

        GLuint r = (GLuint)pixels[0];
        GLuint g = (GLuint)pixels[1];
        GLuint b = (GLuint)pixels[2];

        g = g << 8;
        b = b << 16;

        GLuint id = r | g | b;

        return id;
    }
}