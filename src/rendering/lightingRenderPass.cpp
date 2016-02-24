#include "lightingRenderPass.h"

namespace phi
{
        lightingRenderPass::lightingRenderPass(phi::gBufferRenderPass* gBufferPass, phi::gl* gl, size_t w, size_t h) :
            _w(w),
            _h(h),
            _gl(gl)
        {
            createQuad();

            std::vector<std::string> attribs;
            attribs.push_back("inPosition");
            attribs.push_back("inTexCoord");

            shader = _gl->shadersManager->load("lightingPass", attribs);
            shader->addUniform(0, "textureArrays");

            auto rtAddresses = phi::rtAddresses();
            rtAddresses.rt0Unit = gBufferPass->targets[0]->textureAddress.unit;
            rtAddresses.rt1Unit = gBufferPass->targets[1]->textureAddress.unit;
            rtAddresses.rt2Unit = gBufferPass->targets[2]->textureAddress.unit;
            rtAddresses.rt3Unit = gBufferPass->targets[3]->textureAddress.unit;

            rtAddresses.rt0Page = gBufferPass->targets[0]->textureAddress.page;
            rtAddresses.rt1Page = gBufferPass->targets[1]->textureAddress.page;
            rtAddresses.rt2Page = gBufferPass->targets[2]->textureAddress.page;
            rtAddresses.rt3Page = gBufferPass->targets[3]->textureAddress.page;

            _rtsBuffer = new buffer(bufferTarget::uniform);

            _rtsBuffer->storage(
                sizeof(phi::rtAddresses),
                &rtAddresses,
                bufferStorageUsage::write);

            _rtsBuffer->bindBufferBase(2);
        }
    
        void lightingRenderPass::createQuad()
        {
            _quad = geometry::quad();

            glCreateVertexArrays(1, &_quadVao);
            glBindVertexArray(_quadVao);

            std::vector<vertexAttrib> attribs;
            attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
            attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

            auto verticesCount = _quad->verticesCount;
            vertex* data = new vertex[verticesCount];
            for (auto i = 0; i < verticesCount; i++)
            {
                data[i] = _quad->vboData[i];
                data[i].position *= 2;
            }

            _quadVbo = new vertexBuffer(attribs);
            _quadVbo->storage(_quad->vboSize, data, bufferStorageUsage::write);

            _quadEbo = new buffer(bufferTarget::element);
            _quadEbo->storage(_quad->eboSize, _quad->eboData, bufferStorageUsage::write);

            glBindVertexArray(0);
        }

        void lightingRenderPass::renderQuad()
        {
            glBindVertexArray(_quadVao);
            shader->bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            shader->unbind();
            glBindVertexArray(0);
        }

        void lightingRenderPass::update()
        {
            shader->bind();

            if (_gl->currentState.useBindlessTextures)
                shader->setUniform(0, _gl->texturesManager->handles);
            else
                shader->setUniform(0, _gl->texturesManager->units);

            shader->unbind();
        }

        void lightingRenderPass::render()
        {
            glDisable(GL_DEPTH_TEST);

            /*gBufferPass->framebuffer->bindForReading(gBufferPass->targets[0]);
            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);*/

            glClear(GL_COLOR_BUFFER_BIT);

            renderQuad();
        }
}