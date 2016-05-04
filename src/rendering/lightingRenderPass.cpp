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

        auto targets = gBufferPass->getTargets();
        rtAddresses.rt0Unit = targets[0]->textureAddress.unit;
        rtAddresses.rt1Unit = targets[1]->textureAddress.unit;
        rtAddresses.rt2Unit = targets[2]->textureAddress.unit;
        rtAddresses.rt3Unit = targets[3]->textureAddress.unit;

        rtAddresses.rt0Page = targets[0]->textureAddress.page;
        rtAddresses.rt1Page = targets[1]->textureAddress.page;
        rtAddresses.rt2Page = targets[2]->textureAddress.page;
        rtAddresses.rt3Page = targets[3]->textureAddress.page;

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
        _quad = geometry::quad();

        glCreateVertexArrays(1, &_quadVao);
        glError::check();

        glBindVertexArray(_quadVao);
        glError::check();

        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        auto verticesCount = _quad->verticesCount;
        vertex* data = new vertex[verticesCount];
        for (uint i = 0; i < verticesCount; i++)
        {
            data[i] = _quad->vboData[i];
            data[i].position *= 2;
        }

        _quadVbo = new vertexBuffer(attribs);
        _quadVbo->storage(_quad->vboSize, data, bufferStorageUsage::write);

        _quadEbo = new buffer(bufferTarget::element);
        _quadEbo->storage(_quad->eboSize, _quad->eboData, bufferStorageUsage::write);

        glBindVertexArray(0);
        glError::check();

        safeDeleteArray(data);
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