#include <precompiled.h>

#include "planeGridPass.h"

namespace phi
{
    planeGridPass::planeGridPass(phi::gl* gl, size_t w, size_t h) :
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

    planeGridPass::~planeGridPass()
    {
        safeDelete(_quadVbo);
        safeDelete(_quadEbo);
    }

    void planeGridPass::createQuad()
    {
        _quad = geometry::quad();

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

    void planeGridPass::renderQuad()
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

    void planeGridPass::setTexture(texture* texture)
    {
        texture->wrapMode = GL_REPEAT;
        texture->minFilter = GL_LINEAR_MIPMAP_LINEAR;
        texture->magFilter = GL_LINEAR;
        _textureAddress = _gl->texturesManager->add(texture);
    }

    void planeGridPass::show()
    {
        _beginSeconds = (float)time::totalSeconds;
        _showing = true;
    }

    void planeGridPass::hide()
    {
        _endSeconds = (float)time::totalSeconds;
        _showing = false;
    }

    void planeGridPass::update()
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

    void planeGridPass::render()
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