#include "defaultFrameBuffer.h"

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}