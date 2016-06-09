#pragma once
#include <phi.h>
#include <core\size.h>
#include <core\resolution.h>

#include "renderingApi.h"
#include "texturesManager.h"
#include "framebuffer.h"

namespace phi
{
    class framebufferBuilder
    {
    private:
        struct framebufferAttachment
        {
            framebufferAttachment(
                const string& name, 
                GLenum attachment, 
                const textureLayout& layout) :
                name(name),
                attachment(attachment),
                layout(layout)
            {
            }

            const string name;
            GLenum attachment;
            const textureLayout layout;
        };

    private:
        framebuffer* _framebuffer;
        resolution _resolution;

        map<std::tuple<GLenum, GLenum>, uint> _formats;
        map<std::tuple<GLenum, GLenum>, textureLayout> _layouts;
        vector<framebufferAttachment> _attatchments;

    private:
        framebufferBuilder(framebuffer* framebuffer, resolution resolution);

        textureContainer* reserveContainer(sizeui size, textureLayout layout);
        renderTarget* createRenderTarget(framebufferAttachment& attatchment);

    public:
        RENDERING_API static framebufferBuilder newFramebuffer(resolution resolution);
        RENDERING_API framebufferBuilder with(string renderTargetName, GLenum attachment, GLenum internalFormat, GLenum dataFormat);
        RENDERING_API framebuffer* build();
    };
}