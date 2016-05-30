#include <precompiled.h>
#include "selectionRenderPass.h"

#include <core\time.h>

namespace phi
{
    selectionRenderPass::selectionRenderPass(renderTargetsAddresses& rtAddresses, const gl* const gl, float w, float h) :
        _gl(gl),
        _quad(nullptr),
        _quadVbo(nullptr),
        _quadEbo(nullptr),
        _rtsBuffer(nullptr),
        _w(w),
        _h(h)
    {
        createQuad();

        vector<string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        _program = _gl->shadersManager->load("selectionPass", attribs);
        _program->addUniform(0, "textureArrays");
        _program->addUniform(1, "resolution");
        _program->addUniform(2, "offset");
        _program->addUniform(3, "time");

        _rtsBuffer = new buffer<renderTargetsAddresses>(bufferTarget::uniform);

        _rtsBuffer->storage(
            sizeof(renderTargetsAddresses),
            &rtAddresses,
            bufferStorageUsage::write);

        _rtsBuffer->bindBufferBase(2);
    }

    selectionRenderPass::~selectionRenderPass()
    {
        safeDelete(_rtsBuffer);
        safeDelete(_quad);
        safeDelete(_quadVbo);
        safeDelete(_quadEbo);
    }

    void selectionRenderPass::createQuad()
    {
        _quad = geometry::createQuad(2.0f);

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

    void selectionRenderPass::update()
    {
        const float contourOffset = 0.5f;
        _program->bind();

        if (_gl->currentState.useBindlessTextures)
            _program->setUniform(0, _gl->texturesManager->handles);
        else
            _program->setUniform(0, _gl->texturesManager->units);

        _program->setUniform(1, vec2(_w, _h));
        _program->setUniform(2, contourOffset);
        _program->setUniform(3, static_cast<float>(time::totalSeconds));

        _program->unbind();
    }

    void selectionRenderPass::render()
    {
        glEnable(GL_BLEND);
        glError::check();

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glError::check();

        glBindVertexArray(_quadVao);
        glError::check();

        _program->bind();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glError::check();
        
        _program->unbind();

        glBindVertexArray(0);
        glError::check();

        glDisable(GL_BLEND);
        glError::check();
    }
}
