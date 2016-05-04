#include <precompiled.h>
#include "selectionRenderPass.h"
#include "lightingRenderPass.h" //REMOVE THIS BITCH

namespace phi
{
    selectionRenderPass::selectionRenderPass(const gBufferRenderPass* const gBufferPass, const gl* const gl, size_t w, size_t h) :
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

        _shader = _gl->shadersManager->load("selectionPass", attribs);
        _shader->addUniform(0, "textureArrays");
        _shader->addUniform(1, "resolution");
        _shader->addUniform(2, "offset");

        auto rtAddresses = phi::rtAddresses();
        rtAddresses.rt0Unit = gBufferPass->rt0->textureAddress.unit;
        rtAddresses.rt1Unit = gBufferPass->rt1->textureAddress.unit;
        rtAddresses.rt2Unit = gBufferPass->rt2->textureAddress.unit;
        rtAddresses.rt3Unit = gBufferPass->rt3->textureAddress.unit;
        rtAddresses.depthUnit = gBufferPass->depth->textureAddress.unit;

        rtAddresses.rt0Page = gBufferPass->rt0->textureAddress.page;
        rtAddresses.rt1Page = gBufferPass->rt1->textureAddress.page;
        rtAddresses.rt2Page = gBufferPass->rt2->textureAddress.page;
        rtAddresses.rt3Page = gBufferPass->rt3->textureAddress.page;
        rtAddresses.depthPage = gBufferPass->depth->textureAddress.page;

        _rtsBuffer = new buffer(bufferTarget::uniform);

        _rtsBuffer->storage(
            sizeof(phi::rtAddresses),
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
        
        _quadVbo = new vertexBuffer(attribs);
        _quadVbo->storage(_quad->vboSize, _quad->vboData, bufferStorageUsage::write);

        _quadEbo = new buffer(bufferTarget::element);
        _quadEbo->storage(_quad->eboSize, _quad->eboData, bufferStorageUsage::write);

        glBindVertexArray(0);
        glError::check();
    }

    void selectionRenderPass::update()
    {
        _shader->bind();

        if (_gl->currentState.useBindlessTextures)
            _shader->setUniform(0, _gl->texturesManager->handles);
        else
            _shader->setUniform(0, _gl->texturesManager->units);

        _shader->setUniform(1, vec2(_w, _h));
        _shader->setUniform(2, 0.5f);

        _shader->unbind();
    }

    void selectionRenderPass::render()
    {
        glEnable(GL_BLEND);
        glError::check();

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glError::check();

        glBindVertexArray(_quadVao);
        glError::check();

        _shader->bind();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glError::check();
        
        _shader->unbind();

        glBindVertexArray(0);
        glError::check();

        glDisable(GL_BLEND);
        glError::check();
    }
}
