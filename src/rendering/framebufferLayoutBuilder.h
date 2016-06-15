#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\size.h>

#include "texturesManager.h"
#include "framebuffer.h"

namespace phi
{
    class framebufferLayoutBuilder
    {
    private:
        framebufferLayout* _framebufferLayout;

        /*
        map<std::tuple<GLenum, GLenum>, uint> _formats;
        map<std::tuple<GLenum, GLenum>, textureLayout> _textureLayouts;
        */

    private:
        framebufferLayoutBuilder(framebufferLayout* framebufferLayout);

        //textureContainer* reserveContainer(sizeui size, textureLayout layout);
        //renderTarget* createRenderTarget(renderTargetLayout& attatchment);

    public:
        RENDERING_API static framebufferLayoutBuilder newFramebufferLayout();
        RENDERING_API framebufferLayoutBuilder with(string renderTargetName, GLenum attachment, GLenum internalFormat, GLenum dataFormat);
        RENDERING_API framebufferLayout* build();
    };
}