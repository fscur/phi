#include <precompiled.h>
#include "renderer.h"

namespace phi
{
    renderer::renderer(gl * gl, float w, float h) :
        _gl(gl),
        _w(w),
        _h(h)
    {
        createRenderTargets();

        _gBufferRenderPass = new gBufferRenderPass(gl);
        _lightingRenderPass = new lightingRenderPass(_rtAddresses, gl, w, h);
        _selectionRenderPass = new selectionRenderPass(_rtAddresses, gl, w, h);
        _planeGridRenderPass = new planeGridRenderPass(gl, w, h);
        _defaultFramebuffer = new framebuffer(true);
    }

    renderer::~renderer()
    {
        safeDelete(_gBufferFramebuffer);
        safeDelete(_defaultFramebuffer);
        safeDelete(_finalImageFramebuffer);

        safeDelete(rt0);
        safeDelete(rt1);
        safeDelete(rt2);
        safeDelete(rt3);
        safeDelete(depth);
        safeDelete(_finalImageRT);

        safeDelete(_gBufferRenderPass);
        safeDelete(_lightingRenderPass);
        safeDelete(_selectionRenderPass);
        safeDelete(_planeGridRenderPass);
    }

    void renderer::createRenderTargets()
    {
        auto reserveContainer = [&](GLenum internalFormat, size_t size)
        {
            auto layout = phi::textureContainerLayout();
            layout.w = static_cast<GLsizei>(_w);
            layout.h = static_cast<GLsizei>(_h);
            layout.levels = 1;
            layout.internalFormat = internalFormat;
            layout.wrapMode = GL_REPEAT;
            layout.minFilter = GL_NEAREST;
            layout.magFilter = GL_NEAREST;

            _gl->texturesManager->reserveContainer(layout, size);
        };

        auto createRenderTarget = [&](
            GLenum attachment,
            GLenum internalFormat,
            GLenum dataFormat)
        {
            auto texture = new phi::texture(static_cast<uint>(_w), static_cast<uint>(_h));
            texture->internalFormat = internalFormat;
            texture->dataFormat = dataFormat;
            texture->minFilter = GL_NEAREST;
            texture->magFilter = GL_NEAREST;
            texture->generateMipmaps = false;
            texture->data = 0;

            auto textureAddress = _gl->texturesManager->add(texture);
            return new phi::renderTarget(
                attachment,
                static_cast<GLint>(_w),
                static_cast<GLint>(_h),
                textureAddress,
                texture);
        };

        reserveContainer(GL_RGBA16F, 3);
        reserveContainer(GL_RGBA8, 1);
        reserveContainer(GL_DEPTH_COMPONENT32, 1);

        rt0 = createRenderTarget(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA);
        rt1 = createRenderTarget(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA);
        rt2 = createRenderTarget(GL_COLOR_ATTACHMENT2, GL_RGBA16F, GL_RGBA);
        rt3 = createRenderTarget(GL_COLOR_ATTACHMENT3, GL_RGBA8, GL_RGBA);
        depth = createRenderTarget(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);

        _gBufferFramebuffer = new framebuffer();
        _gBufferFramebuffer->add(rt0);
        _gBufferFramebuffer->add(rt1);
        _gBufferFramebuffer->add(rt2);
        _gBufferFramebuffer->add(rt3);
        _gBufferFramebuffer->add(depth);

        _rtAddresses.rt0Unit = rt0->textureAddress.unit;
        _rtAddresses.rt1Unit = rt1->textureAddress.unit;
        _rtAddresses.rt2Unit = rt2->textureAddress.unit;
        _rtAddresses.rt3Unit = rt3->textureAddress.unit;
        _rtAddresses.depthUnit = depth->textureAddress.unit;
        _rtAddresses.rt0Page = rt0->textureAddress.page;
        _rtAddresses.rt1Page = rt1->textureAddress.page;
        _rtAddresses.rt2Page = rt2->textureAddress.page;
        _rtAddresses.rt3Page = rt3->textureAddress.page;
        _rtAddresses.depthPage = depth->textureAddress.page;

        auto finalImageTexture = new texture(
            static_cast<uint>(_w),
            static_cast<uint>(_h),
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr,
            GL_TEXTURE_2D,
            GL_RGBA8,
            GL_CLAMP_TO_EDGE,
            GL_LINEAR_MIPMAP_LINEAR,
            GL_LINEAR,
            true);

        auto finalImageTexAddress = _gl->texturesManager->add(finalImageTexture);

        _finalImageRT = new renderTarget(
            GL_COLOR_ATTACHMENT0,
            static_cast<GLint>(_w),
            static_cast<GLint>(_h),
            finalImageTexAddress,
            finalImageTexture);

        _finalImageFramebuffer = new framebuffer();
        _finalImageFramebuffer->add(_finalImageRT);
        _finalImageFramebuffer->add(depth);
    }

    void renderer::blitGBufferDepthToDefaultFrameBuffer()
    {
        _gBufferFramebuffer->bindForReading();

        auto w = static_cast<GLint>(_w);
        auto h = static_cast<GLint>(_h);

        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glError::check();
    }

    void renderer::clearGBufferFrameBuffer()
    {
        const auto selectionRenderTargetNumber = 3;
        auto selectionClearColor = 0.0f;

        glDepthMask(GL_TRUE);
        glError::check();
        glEnable(GL_DEPTH_TEST);
        glError::check();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glError::check();
        glClearBufferfv(GL_COLOR, selectionRenderTargetNumber, &selectionClearColor);
        glError::check();
    }

    void renderer::generateFinalImageMipMaps()
    {
        glActiveTexture(GL_TEXTURE0 + _finalImageRT->textureAddress.unit);
        glError::check();
        glBindTexture(GL_TEXTURE_2D_ARRAY, _finalImageRT->textureAddress.containerId);
        glError::check();
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        glError::check();
    }

    void renderer::render(const vector<batch*>& batches)
    {
        _gBufferFramebuffer->bindForDrawing();
        clearGBufferFrameBuffer();

        _gBufferRenderPass->render(batches);

        _gBufferFramebuffer->unbind(GL_FRAMEBUFFER);
        blitGBufferDepthToDefaultFrameBuffer();

        _finalImageFramebuffer->bindForDrawing();

        _lightingRenderPass->render();
        _planeGridRenderPass->render();
        _selectionRenderPass->render();

        _finalImageFramebuffer->blitToDefault(_finalImageRT);
        generateFinalImageMipMaps();
    }

    void renderer::update()
    {
        _gBufferRenderPass->update();
        _lightingRenderPass->update();
        _selectionRenderPass->update();
        _planeGridRenderPass->update();
    }
}