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
            framebufferAttachment(GLenum attachment, GLenum internalFormat, GLenum dataFormat) :
                attachment(attachment),
                internalFormat(internalFormat),
                dataFormat(dataFormat)
            {
            }

            GLenum attachment;
            GLenum internalFormat;
            GLenum dataFormat;
        };

    private:
        framebuffer* _framebuffer;
        gl* _gl;
        float _width;
        float _height;

        map<GLenum, uint> _formatCounts;
        vector<framebufferAttachment> _attatchments;

    private:
        framebufferBuilder(framebuffer* framebuffer, gl* gl, float width, float height);

        void reserveContainer(GLenum internalFormat, size_t size);
        renderTarget* createRenderTarget(framebufferAttachment& attatchment);

    public:
        RENDERING_API static framebufferBuilder newFramebuffer(gl* gl, float width, float height);
        RENDERING_API framebufferBuilder with(GLenum attachment, GLenum internalFormat, GLenum dataFormat);
        RENDERING_API framebuffer* build();
    };
}