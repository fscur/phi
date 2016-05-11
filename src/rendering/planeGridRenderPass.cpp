#include <precompiled.h>

#include "planeGridRenderPass.h"

namespace phi
{
    planeGridRenderPass::planeGridRenderPass(phi::gl* gl, float w, float h) :
        _gl(gl),
        _w(w),
        _h(h),
        _quad(nullptr),
        _quadVao(0u),
        _quadVbo(nullptr),
        _quadEbo(nullptr),
        _showing(false),
        shader(nullptr)
    {
        createQuad();

        vector<string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        shader = _gl->shadersManager->load("planeGridPass", attribs);

        shader->addUniform(0, "m");
        shader->addUniform(1, "radius");
        shader->addUniform(2, "scale");
        shader->addUniform(3, "textureArrays");
        shader->addUniform(4, "textureArrayIndex");
        shader->addUniform(5, "texturePageIndex");
        shader->addUniform(6, "showing");
        shader->addUniform(7, "radiusHiding");

        _scale = 1000.0f;
        transform.setLocalSize(vec3(_scale));
    }

    planeGridRenderPass::~planeGridRenderPass()
    {
        safeDelete(_quad);
        safeDelete(_quadVbo);
        safeDelete(_quadEbo);
		safeDelete(_gridTexture);
    }

    void planeGridRenderPass::createQuad()
    {
        _quad = geometry::createQuad(1.0f);

        glCreateVertexArrays(1, &_quadVao);
        glError::check();

        glBindVertexArray(_quadVao);
        glError::check();

        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        _quadVbo = new vertexBuffer(attribs);
        _quadVbo->storage(_quad->vboSize, _quad->vboData, bufferStorageUsage::write);

        _quadEbo = new buffer(bufferTarget::element);
        _quadEbo->storage(_quad->eboSize, _quad->eboData, bufferStorageUsage::write);

        glBindVertexArray(0);
        glError::check();
    }

    void planeGridRenderPass::renderQuad()
    {
        glBindVertexArray(_quadVao);
        glError::check();

        shader->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glError::check();
        shader->unbind();

        glBindVertexArray(0);
        glError::check();
    }

    void planeGridRenderPass::setImage(image* image)
    {
		_gridTexture = new phi::texture(image);
		_gridTexture->type = GL_TEXTURE_2D;
		_gridTexture->internalFormat = GL_RGBA8;
        _gridTexture->wrapMode = GL_REPEAT;
        _gridTexture->minFilter = GL_LINEAR_MIPMAP_LINEAR;
        _gridTexture->magFilter = GL_LINEAR;
        _textureAddress = _gl->texturesManager->add(_gridTexture);
    }

    void planeGridRenderPass::show()
    {
        _beginSeconds = (float)time::totalSeconds;
        _showing = true;
    }

    void planeGridRenderPass::hide()
    {
        _endSeconds = (float)time::totalSeconds;
        _showing = false;
    }

    void planeGridRenderPass::update()
    {
        shader->bind();
        shader->setUniform(0, transform.getModelMatrix());
        shader->setUniform(1, ((float)phi::time::totalSeconds - _beginSeconds) * 20.0f);
        shader->setUniform(2, _scale);

        if (_gl->currentState.useBindlessTextures)
            shader->setUniform(3, _gl->texturesManager->handles);
        else
            shader->setUniform(3, _gl->texturesManager->units);

        shader->setUniform(4, _textureAddress.unit);
        shader->setUniform(5, _textureAddress.page);
        shader->setUniform(6, _showing);
        shader->setUniform(7, !_showing * ((float)phi::time::totalSeconds - _endSeconds) * 20.0f);

        shader->unbind();
    }

    void planeGridRenderPass::render()
    {
        glEnable(GL_DEPTH_TEST);
        glError::check();
        glEnable(GL_BLEND);
        glError::check();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glError::check();
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        renderQuad();
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glError::check();
    }
}