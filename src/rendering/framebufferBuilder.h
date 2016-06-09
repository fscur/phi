#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\resolution.h>

#include "gl.h"
#include "framebuffer.h"

namespace phi
{
    class framebufferBuilder
    {
    private:
        struct framebufferAttachment
        {
            framebufferAttachment(GLenum attachment, textureLayout layout) :
                attachment(attachment),
                layout(layout)
            {
            }

            GLenum attachment;
            textureLayout layout;
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
        RENDERING_API framebufferBuilder with(GLenum attachment, GLenum internalFormat, GLenum dataFormat);
        RENDERING_API framebuffer* build();
    };
}