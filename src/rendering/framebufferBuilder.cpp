#include <precompiled.h>
#include "framebufferBuilder.h"

namespace phi
{
    framebufferBuilder::framebufferBuilder(framebuffer * framebuffer, gl* gl, float width, float height) :
        _framebuffer(framebuffer),
        _gl(gl),
        _width(width),
        _height(height)
    {
    }

    textureContainer* framebufferBuilder::reserveContainer(sizeui size, textureLayout layout)
    {
        return _gl->texturesManager->reserveContainer(size, layout);
    }

    renderTarget* framebufferBuilder::createRenderTarget(framebufferAttachment& attachment)
    {
        auto texture = new phi::texture(
            static_cast<uint>(_width),
            static_cast<uint>(_height),
            attachment.layout);

        auto textureAddress = _gl->texturesManager->get(texture);
        return new phi::renderTarget(
            attachment.attachment,
            static_cast<GLint>(_width),
            static_cast<GLint>(_height),
            textureAddress,
            texture);
    }

    framebufferBuilder framebufferBuilder::newFramebuffer(gl* gl, float width, float height)
    {
        return framebufferBuilder(new framebuffer(), gl, width, height);
    }

    framebufferBuilder framebufferBuilder::with(GLenum attachment, GLenum internalFormat, GLenum dataFormat)
    {
        textureLayout layout;

        auto key = std::tuple<GLenum, GLenum>(internalFormat, dataFormat);

        if (contains(_formats, key))
        {
            _formats[key]++;
            layout = _layouts[key];
        }
        else
        {
            layout = textureLayout();
            layout.dataFormat = dataFormat;
            layout.dataType = GL_UNSIGNED_BYTE;
            layout.internalFormat = internalFormat;
            layout.wrapMode = GL_REPEAT;
            layout.minFilter = GL_NEAREST;
            layout.magFilter = GL_NEAREST;

            _formats[key]++;
            _layouts[key] = layout;
        }

        _attatchments.push_back(framebufferAttachment(attachment, layout));

        return *this;
    }

    framebuffer* framebufferBuilder::build()
    {
        for (auto& pair : _formats)
            reserveContainer(sizeui(static_cast<uint>(_width), static_cast<uint>(_height), pair.second), _layouts[pair.first]);

        for (auto& attachment : _attatchments)
            _framebuffer->add(createRenderTarget(attachment));

        return _framebuffer;
    }
}