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
        program(nullptr)
    {
        createQuad();

        vector<string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        program = _gl->shadersManager->load("planeGridPass", attribs);

        program->addUniform(0, "m");
        program->addUniform(1, "radius");
        program->addUniform(2, "scale");
        program->addUniform(3, "textureArrays");
        program->addUniform(4, "textureArrayIndex");
        program->addUniform(5, "texturePageIndex");
        program->addUniform(6, "showing");
        program->addUniform(7, "radiusHiding");

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

        _quadVbo = new vertexBuffer<vertex>(attribs);
        _quadVbo->storage(_quad->vboSize, _quad->vboData, bufferStorageUsage::write);

        _quadEbo = new buffer<uint>(bufferTarget::element);
        _quadEbo->storage(_quad->eboSize, _quad->eboData, bufferStorageUsage::write);

        glBindVertexArray(0);
        glError::check();
    }

    void planeGridRenderPass::renderQuad()
    {
        glBindVertexArray(_quadVao);
        glError::check();

        program->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glError::check();
        program->unbind();

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
        _textureAddress = _gl->texturesManager->get(_gridTexture);
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
        program->bind();
        program->setUniform(0, transform.getModelMatrix());
        program->setUniform(1, ((float)phi::time::totalSeconds - _beginSeconds) * 20.0f);
        program->setUniform(2, _scale);

        if (_gl->currentState.useBindlessTextures)
            program->setUniform(3, _gl->texturesManager->handles);
        else
            program->setUniform(3, _gl->texturesManager->units);

        program->setUniform(4, _textureAddress.unit);
        program->setUniform(5, _textureAddress.page);
        program->setUniform(6, _showing);
        program->setUniform(7, !_showing * ((float)phi::time::totalSeconds - _endSeconds) * 20.0f);

        program->unbind();
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