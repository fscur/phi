#include "renderTarget.h"

namespace phi
{
	renderTarget::renderTarget()
	{
		_isBound = false;
		_isInitialized = false;
		_id = 0;
		_clearColor = color::black;
		_x = 0;
		_y = 0;
		_size = size<GLuint>(0, 0);
	}

	renderTarget::renderTarget(size<GLuint> size, color clearColor)
	{
		_x = 0;
		_y = 0;
		_size = size;
		_isBound = false;
		_isInitialized = false;
		_id = 0;
		_clearColor = clearColor;
		glGenFramebuffers(1, &_id);
	}

	renderTarget::~renderTarget()
	{
		glDeleteFramebuffers(1, &_id);
	}

	bool renderTarget::init()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _id);

		int error = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (error != GL_FRAMEBUFFER_COMPLETE)
		{
			LOG("Error initializing Render Target");
			return false;
		}

		return true;
	}

	void renderTarget::setSize(size<GLuint> value)
	{
		if (!_isInitialized)
		{
			LOG("RenderTarget not initialized.");
			return;
		}

		_size = value;

		if (_isBound)
		{
			unbind();
			bind();
		}
		else
			glViewport(0,0, _size.width, _size.height);
	}

	void renderTarget::setClearColor(color value)
	{
		if (!_isInitialized)
		{
			LOG("RenderTarget not initialized.");
			return;
		}

		_clearColor = value; 
		glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
	}

	void renderTarget::bind()
	{
		if (!_isInitialized)
		{
			LOG("RenderTarget not initialized.");
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, _id);
		_isBound = true;
	}

	void renderTarget::bindForWriting()
	{
		if (!_isInitialized)
		{
			LOG("RenderTarget not initialized.");
			return;
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _id);
	}

	void renderTarget::bindForReading()
	{
		if (!_isInitialized)
		{
			LOG("RenderTarget not initialized.");
			return;
		}

		glBindFramebuffer(GL_READ_FRAMEBUFFER, _id);
	}

	void renderTarget::setViewport(GLuint x, GLuint y, size<GLuint> size)
	{
		if (!_isInitialized)
		{
			LOG("RenderTarget not initialized.");
			return;
		}

		_x = x;
		_y = y;
		_size = size;

		glViewport(_x, _y, _size.width, _size.height);
	}

	void renderTarget::clear()
	{
		if (!_isInitialized)
		{
			LOG("RenderTarget not initialized.");
			return;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void renderTarget::unbind()
	{
		if (!_isInitialized)
		{
			LOG("RenderTarget not initialized.");
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		_isBound = false;
	}
}