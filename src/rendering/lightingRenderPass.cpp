#include <precompiled.h>
#include "lightingRenderPass.h"

namespace phi
{
    lightingRenderPass::lightingRenderPass(gBufferRenderPass* gBufferPass, gl* gl, float w, float h) :
        _gl(gl),
        _quad(nullptr),
        _shader(nullptr),
        _quadVbo(nullptr),
        _quadEbo(nullptr),
        _rtsBuffer(nullptr),
        _w(w),
        _h(h),
        _quadVao(0u)
    {
        createQuad();

        vector<string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        _shader = _gl->shadersManager->load("lightingPass", attribs);
        _shader->addUniform(0, "textureArrays");

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

    lightingRenderPass::~lightingRenderPass()
    {
        safeDelete(_rtsBuffer);
        safeDelete(_quad);
        safeDelete(_quadVbo);
        safeDelete(_quadEbo);
    }

    void lightingRenderPass::createQuad()
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

    void lightingRenderPass::update()
    {
        _shader->bind();

        if (_gl->currentState.useBindlessTextures)
            _shader->setUniform(0, _gl->texturesManager->handles);
        else
            _shader->setUniform(0, _gl->texturesManager->units);

        _shader->unbind();
    }

    void lightingRenderPass::render()
    {
        glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(_quadVao);
		glError::check();

		_shader->bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glError::check();
		_shader->unbind();

		glBindVertexArray(0);
		glError::check();

		glEnable(GL_DEPTH_TEST);
    }
}