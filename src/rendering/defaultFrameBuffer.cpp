#include <phi/rendering/defaultFrameBuffer.h>

namespace phi
{
	defaultFrameBuffer::defaultFrameBuffer(color clearColor) : frameBuffer("default", clearColor)
	{
	}

	defaultFrameBuffer::~defaultFrameBuffer()
	{
        frameBuffer::~frameBuffer();
	}

	void defaultFrameBuffer::init()
    {   
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glEnable(GL_DEPTH_TEST);
    }

    void defaultFrameBuffer::clear()
    {
        glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}