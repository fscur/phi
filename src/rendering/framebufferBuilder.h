#pragma once
#include <phi.h>
#include "renderingApi.h"

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
        gl* _gl;
        float _width;
        float _height;

        map<std::tuple<GLenum, GLenum>, uint> _formats;
        map<std::tuple<GLenum, GLenum>, textureLayout> _layouts;
        vector<framebufferAttachment> _attatchments;

    private:
        framebufferBuilder(framebuffer* framebuffer, gl* gl, float width, float height);

        textureContainer* reserveContainer(sizeui size, textureLayout layout, size_t pages);
        renderTarget* createRenderTarget(framebufferAttachment& attatchment);

    public:
        RENDERING_API static framebufferBuilder newFramebuffer(gl* gl, float width, float height);
        RENDERING_API framebufferBuilder with(GLenum attachment, GLenum internalFormat, GLenum dataFormat);
        RENDERING_API framebuffer* build();
    };
}