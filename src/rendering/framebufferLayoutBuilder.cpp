#include <precompiled.h>
#include "framebufferLayoutBuilder.h"

namespace phi
{
    framebufferLayoutBuilder::framebufferLayoutBuilder(framebufferLayout* framebufferLayout) :
        _framebufferLayout(framebufferLayout)
    {
    }

    /* textureContainer* framebufferLayoutBuilder::reserveContainer(sizeui size, textureLayout layout)
     {
         return texturesManager::reserveContainer(size, layout);
     }*/

     /*renderTarget* framebufferLayoutBuilder::createRenderTarget(renderTargetLayout& attachment)
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
     }*/

    framebufferLayoutBuilder framebufferLayoutBuilder::newFramebufferLayout()
    {
        return framebufferLayoutBuilder(new framebufferLayout());
    }

    framebufferLayoutBuilder framebufferLayoutBuilder::with(string renderTargetName, GLenum attachment, GLenum internalFormat, GLenum dataFormat)
    {
        /*textureLayout layout;

        auto key = std::tuple<GLenum, GLenum>(internalFormat, dataFormat);

        if (contains(_formats, key))
        {
            _formats[key]++;
            layout = _textureLayouts[key];
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
            _textureLayouts[key] = layout;
        }

        _framebufferLayout->add(attachment, renderTargetLayout(renderTargetName, layout));

        return *this;*/

        textureLayout layout = textureLayout();
        layout.dataFormat = dataFormat;
        layout.dataType = GL_UNSIGNED_BYTE;
        layout.internalFormat = internalFormat;
        layout.wrapMode = GL_REPEAT;
        layout.minFilter = GL_NEAREST;
        layout.magFilter = GL_NEAREST;

        _framebufferLayout->add(attachment, new renderTargetLayout(renderTargetName, layout));

        return *this;
    }

    framebufferLayout* framebufferLayoutBuilder::build()
    {
        /*auto width = static_cast<uint>(_resolution.width);
        auto height = static_cast<uint>(_resolution.height);

        for (auto& pair : _formats)
            reserveContainer(sizeui(width, height, pair.second), _textureLayouts[pair.first]);*/

        return _framebufferLayout;
    }
}