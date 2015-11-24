#include <phi/rendering/defaultFrameBuffer.h>

namespace phi
{
	defaultFrameBuffer::defaultFrameBuffer(size<GLuint> size, color clearColor) : frameBuffer("default", size, clearColor)
	{
	}

	defaultFrameBuffer::~defaultFrameBuffer()
	{
	}

	void defaultFrameBuffer::init()
    {   
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

    void defaultFrameBuffer::clear()
    {
        glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}