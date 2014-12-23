#include "shadowMapRenderTarget.h"

namespace phi
{
	shadowMapRenderTarget::shadowMapRenderTarget(GLuint width, GLuint height) :
        renderTarget(size<GLuint>(width, height), color::black)
    {   
    }

    shadowMapRenderTarget::~shadowMapRenderTarget()
    {
        renderTarget::~renderTarget();
        _shadowMap->release();
    }

    bool shadowMapRenderTarget::init()
    {   
		_shadowMap = texture::create(_size, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
		_shadowMap->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		_shadowMap->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_shadowMap->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		_shadowMap->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glBindFramebuffer(GL_FRAMEBUFFER, getId());

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowMap->getId(), 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (Status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::string msg = &"FB error, status: 0x%x\n" [Status];
            LOG(msg);
            return false;
        }
        
        _isInitialized = true;

        return true;
    }

    void shadowMapRenderTarget::bind()
    {
        renderTarget::bind();
    }

    void shadowMapRenderTarget::clear()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    texture* shadowMapRenderTarget::getShadowMap()
    {
        return _shadowMap;
    }
}