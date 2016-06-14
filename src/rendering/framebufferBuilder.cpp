#include <precompiled.h>
#include "framebufferBuilder.h"

namespace phi
{
    framebufferBuilder::framebufferBuilder(framebuffer * framebuffer, resolution resolution) :
        _framebuffer(framebuffer),
        _resolution(resolution)
    {
    }

    textureContainer* framebufferBuilder::reserveContainer(sizeui size, textureLayout layout)
    {
        return texturesManager::reserveContainer(size, layout);
    }

    renderTarget* framebufferBuilder::createRenderTarget(framebufferAttachment& attachment)
    {
        auto texture = new phi::texture(
            static_cast<uint>(_resolution.width),
            static_cast<uint>(_resolution.height),
            attachment.layout);

        auto textureAddress = texturesManager::add(texture);
        return new phi::renderTarget(
            attachment.name,
            static_cast<GLint>(_resolution.width),
            static_cast<GLint>(_resolution.height),
            attachment.layout,
            textureAddress);
    }

    framebufferBuilder framebufferBuilder::newFramebuffer(resolution resolution)
    {
        return framebufferBuilder(new framebuffer(), resolution);
    }

    framebufferBuilder framebufferBuilder::with(string renderTargetName, GLenum attachment, GLenum internalFormat, GLenum dataFormat)
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

        _attatchments.push_back(framebufferAttachment(renderTargetName, attachment, layout));

        return *this;
    }

    framebuffer* framebufferBuilder::build()
    {
        auto width = static_cast<uint>(_resolution.width);
        auto height = static_cast<uint>(_resolution.height);

        for (auto& pair : _formats)
            reserveContainer(sizeui(width, height, pair.second), _layouts[pair.first]);

        for (auto& attachment : _attatchments)
            _framebuffer->add(attachment.attachment, createRenderTarget(attachment));

        return _framebuffer;
    }
}