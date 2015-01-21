#include "gBuffer.h"

namespace phi
{
	gBuffer::gBuffer(size<GLuint> size) :
        frameBuffer("gBuffer", size, color::black)
    {
    }

    gBuffer::~gBuffer()
    {
        frameBuffer::~frameBuffer();

        _positionTexture->release();
		_normalTexture->release();
		_ambientTexture->release();
		_diffuseTexture->release();
		_specularTexture->release();
		_shininessTexture->release();
		_finalTexture->release();
		_depthTexture->release();
    }

	void gBuffer::createTextures()
	{	
		_positionTexture = texture::create(_size);
		_positionTexture->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		_positionTexture->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_positionTexture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		_positionTexture->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		_ambientTexture = texture::create(_size);
		_ambientTexture->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		_ambientTexture->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_ambientTexture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		_ambientTexture->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		_normalTexture = texture::create(_size);
		_normalTexture->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		_normalTexture->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_normalTexture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		_normalTexture->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		_diffuseTexture = texture::create(_size);
		_diffuseTexture->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		_diffuseTexture->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_diffuseTexture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		_diffuseTexture->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		_specularTexture = texture::create(_size);
		_specularTexture->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		_specularTexture->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_specularTexture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		_specularTexture->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		_shininessTexture = texture::create(_size);
		_shininessTexture->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		_shininessTexture->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_shininessTexture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		_shininessTexture->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		_finalTexture = texture::create(_size);
		_finalTexture->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		_finalTexture->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_finalTexture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		_finalTexture->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		_depthTexture = texture::create(_size, GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
		_depthTexture->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		_depthTexture->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		_depthTexture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		_depthTexture->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _positionTexture->getId(), 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _normalTexture->getId(), 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _ambientTexture->getId(), 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, _diffuseTexture->getId(), 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, _specularTexture->getId(), 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, _shininessTexture->getId(), 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, _finalTexture->getId(), 0);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _depthTexture->getId(), 0);
	}

    void gBuffer::init()
    {   
        glBindFramebuffer(GL_FRAMEBUFFER, getId());

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        createTextures();

        GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (Status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::string msg = &"FB error, status: 0x%x\n" [Status];
            LOG(msg);
            return;
        }

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        _isInitialized = true;
    }

    void gBuffer::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

	void gBuffer::blit(GLuint buffer, GLuint x, GLuint y, GLsizei width, GLsizei height)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + buffer);

		glBlitFramebuffer(
			0,
			0,
			_size.width,
			_size.height,
			x, 
			y, 
			x + width, 
			y + height, 
			GL_COLOR_BUFFER_BIT, 
			GL_LINEAR);
	}

	void gBuffer::bindForGeomPass()
	{
		bindForDrawing();
		glDrawBuffer(GL_COLOR_ATTACHMENT6);
		glClear(GL_COLOR_BUFFER_BIT);

		GLenum drawBuffers[] = 
		{ 
			GL_COLOR_ATTACHMENT0, 
			GL_COLOR_ATTACHMENT1, 
			GL_COLOR_ATTACHMENT2, 
			GL_COLOR_ATTACHMENT3,
			GL_COLOR_ATTACHMENT4,
			GL_COLOR_ATTACHMENT5
		};

		glDrawBuffers(6, drawBuffers);
	}

	void gBuffer::bindForStencilPass()
	{
		glDrawBuffer(GL_NONE);
	}

	void gBuffer::bindForLightPass()
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT6);
	}
}