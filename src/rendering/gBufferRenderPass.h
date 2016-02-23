#pragma once

#include "framebuffer.h"
#include "batch.h"

namespace phi
{
    /*
    texture* t = texture::create(_viewportSize, GL_RGBA);
    t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    renderTarget* r = _frameBuffers[0]->newRenderTarget(
    "rt0",
    t,
    GL_DRAW_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0);

    _frameBuffers[0]->addRenderTarget(r);

    t = texture::create(_viewportSize, GL_RGBA16F);
    t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    r = _frameBuffers[0]->newRenderTarget(
    "rt1",
    t,
    GL_DRAW_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT1);

    _frameBuffers[0]->addRenderTarget(r);

    t = texture::create(_viewportSize, GL_RGBA16F);
    t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    r = _frameBuffers[0]->newRenderTarget(
    "rt2",
    t,
    GL_DRAW_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT2);

    _frameBuffers[0]->addRenderTarget(r);

    t = texture::create(_viewportSize, GL_RGBA16F);
    t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    r = _frameBuffers[0]->newRenderTarget(
    "rt3",
    t,
    GL_DRAW_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT3);

    _frameBuffers[0]->addRenderTarget(r);

    t = texture::create(_viewportSize, GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
    t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    r = _frameBuffers[0]->newRenderTarget(
    "rt4",
    t,
    GL_DRAW_FRAMEBUFFER,
    GL_DEPTH_STENCIL_ATTACHMENT);

    _frameBuffers[0]->addRenderTarget(r);

    t = renderingSystem::pickingFrameBuffer->getPickingTexture();

    r = _frameBuffers[0]->newRenderTarget(
    "rt5",
    t,
    GL_DRAW_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT4);

    _frameBuffers[0]->addRenderTarget(r);

    _frameBuffers[0]->enable(GL_CULL_FACE);
    _frameBuffers[0]->enable(GL_DEPTH_TEST);
    _frameBuffers[0]->unbind();
    */

    class gBufferRenderPass
    {
    private:
        phi::gl* _gl;
        size_t _w;
        size_t _h;

    public:
        phi::shader* shader;
        std::vector<renderTarget*> targets;
        std::vector<phi::batch*> batches;
        phi::framebuffer* framebuffer;

        gBufferRenderPass(phi::gl* gl, size_t w, size_t h) :
            _w(w),
            _h(h),
            _gl(gl),
            framebuffer(new phi::framebuffer())
        {
            std::vector<std::string> attribs;
            attribs.push_back("inPosition");
            attribs.push_back("inTexCoord");
            attribs.push_back("inNormal");
            attribs.push_back("inTangent");
            attribs.push_back("inMaterialId");
            attribs.push_back("inModelMatrix");

            shader = _gl->shadersManager->load("geometryPass", attribs);
            shader->addUniform(0, "textureArrays");

            auto createTexture = [&](
                GLenum attachment,
                uint w, 
                uint h, 
                GLenum internalFormat, 
                GLenum dataFormat)
            {
                auto texture = new phi::texture(w, h);
                texture->internalFormat = internalFormat;
                texture->dataFormat = dataFormat;
                texture->minFilter = GL_NEAREST;
                texture->magFilter = GL_NEAREST;
                texture->generateMipmaps = false;

                auto rt = new phi::renderTarget(attachment, w, h, _gl->texturesManager->add(texture));
                targets.push_back(rt);
                gBufferRenderPass::framebuffer->add(rt);
            };

            createTexture(GL_COLOR_ATTACHMENT0, w, h, GL_RGBA16F, GL_RGBA);
            createTexture(GL_COLOR_ATTACHMENT1, w, h, GL_RGBA16F, GL_RGBA);
            createTexture(GL_COLOR_ATTACHMENT2, w, h, GL_RGBA16F, GL_RGBA);
            createTexture(GL_DEPTH_ATTACHMENT, w, h, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);
        }

        void update()
        {
            shader->bind();

            if (_gl->currentState.useBindlessTextures)
                shader->setUniform(0, _gl->texturesManager->handles);
            else
                shader->setUniform(0, _gl->texturesManager->units);

            shader->unbind();
        }

        void render()
        {
            framebuffer->bindForDrawing();

            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader->bind();

            for (auto batch : batches)
                batch->render();

            shader->unbind();

            framebuffer->unbind(GL_FRAMEBUFFER);
            framebuffer->bindForReading(targets[3]);
            glBlitFramebuffer(0, 0, _w, _h, 0, 0, _w, _h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        }
    };
}