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

    void framebufferBuilder::reserveContainer(GLenum internalFormat, size_t size)
    {
        auto layout = phi::textureContainerLayout();
        layout.w = static_cast<GLsizei>(_width);
        layout.h = static_cast<GLsizei>(_height);
        layout.levels = 1;
        layout.internalFormat = internalFormat;
        layout.wrapMode = GL_REPEAT;
        layout.minFilter = GL_NEAREST;
        layout.magFilter = GL_NEAREST;

        _gl->texturesManager->reserveContainer(layout, size);
    }

    renderTarget * framebufferBuilder::createRenderTarget(framebufferAttachment& attachment)
    {
            auto texture = new phi::texture(static_cast<uint>(_width), static_cast<uint>(_height));
            texture->internalFormat = attachment.internalFormat;
            texture->dataFormat = attachment.dataFormat;
            texture->minFilter = GL_NEAREST;
            texture->magFilter = GL_NEAREST;
            texture->generateMipmaps = false;
            texture->data = 0;

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
        _formatCounts[internalFormat]++;
        _attatchments.push_back(framebufferAttachment(attachment, internalFormat, dataFormat));

        return *this;
    }

    framebuffer* framebufferBuilder::build()
    {
        for (auto& pair : _formatCounts)
            reserveContainer(pair.first, pair.second);

        for(auto& attachment : _attatchments)
            _framebuffer->add(createRenderTarget(attachment));

        return _framebuffer;
    }
}