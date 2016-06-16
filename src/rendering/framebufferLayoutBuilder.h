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
        RENDERING_API static framebufferLayoutBuilder newFramebufferLayout(const string& name);
        RENDERING_API framebufferLayoutBuilder with(
            const string renderTargetName, 
            const GLenum attachment, 
            const GLenum internalFormat, 
            const GLenum dataFormat);

        RENDERING_API framebufferLayoutBuilder with(const string renderTargetName, const GLenum attachment, const textureLayout& layout);
        RENDERING_API framebufferLayoutBuilder with(const renderTarget* renderTarget, const GLenum attachment);
        RENDERING_API framebufferLayout* build();
    };
}