#include "defaultRenderTarget.h"

namespace phi
{
	defaultRenderTarget::defaultRenderTarget(size<GLuint> size, color clearColor)
    {   
        _id = 0;
        _isInitialized = false;
        _isBound = true;
        _size = size;
        _clearColor = clearColor;
    }

    defaultRenderTarget::~defaultRenderTarget()
    {
    }

    bool defaultRenderTarget::init()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        _isInitialized = true;

        return true;
    }

    void defaultRenderTarget::clear()
    {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}