#include <precompiled.h>
#include "framebufferLayoutBuilder.h"

namespace phi
{
    framebufferLayoutBuilder::framebufferLayoutBuilder(framebufferLayout* framebufferLayout) :
        _framebufferLayout(framebufferLayout)
    {
    }

    framebufferLayoutBuilder framebufferLayoutBuilder::newFramebufferLayout(const string& name)
    {
        return framebufferLayoutBuilder(new framebufferLayout(name));
    }

    framebufferLayoutBuilder framebufferLayoutBuilder::with(
        const string renderTargetName, 
        const GLenum attachment, 
        const GLenum internalFormat, 
        const GLenum dataFormat)
    {
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

    framebufferLayoutBuilder framebufferLayoutBuilder::with(
        const string renderTargetName, 
        const GLenum attachment, 
        const textureLayout& layout)
    {
        _framebufferLayout->add(attachment, new renderTargetLayout(renderTargetName, layout));

        return *this;
    }

    framebufferLayoutBuilder framebufferLayoutBuilder::with(
        const renderTarget* renderTarget, 
        const GLenum attachment)
    {
        _framebufferLayout->add(attachment, new renderTargetLayout(renderTarget->name, renderTarget->texture->layout));

        return *this;
    }

    framebufferLayout* framebufferLayoutBuilder::build()
    {
        return _framebufferLayout;
    }
}