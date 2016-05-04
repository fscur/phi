#include <precompiled.h>
#include "renderer.h"

namespace phi
{
    renderer::renderer(gl * gl, float w, float h) :
        _gl(gl),
        _gBufferRenderPass(new gBufferRenderPass(gl, w, h)),
        _lightingRenderPass(new lightingRenderPass(_gBufferRenderPass, gl, w, h)),
        _selectionRenderPass(new selectionRenderPass(_gBufferRenderPass, gl, w, h)),
        _planeGridRenderPass(new planeGridRenderPass(gl, w, h)),
        _defaultFramebuffer(new framebuffer(true)),
        _finalImageFramebuffer(new framebuffer()),
        _w(w),
        _h(h)
    {
        auto finalImageTexture = new texture(
            static_cast<uint>(w),
            static_cast<uint>(h),
            GL_TEXTURE_2D,
            GL_RGBA8,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr,
            GL_CLAMP_TO_EDGE,
            GL_LINEAR_MIPMAP_LINEAR,
            GL_LINEAR,
            true);

        auto finalImageTexAddress = _gl->texturesManager->add(finalImageTexture);

        _finalImageRT = new renderTarget(
            GL_COLOR_ATTACHMENT0,
            static_cast<GLint>(w),
            static_cast<GLint>(h),
            finalImageTexAddress,
            finalImageTexture);

        _finalImageFramebuffer->add(_finalImageRT);
    }

    renderer::~renderer()
    {
        safeDelete(_defaultFramebuffer);
        safeDelete(_finalImageFramebuffer);
        safeDelete(_gBufferRenderPass);
        safeDelete(_lightingRenderPass);
        safeDelete(_planeGridRenderPass);
        safeDelete(_finalImageRT);
    }

    void renderer::render()
    {
        _gBufferRenderPass->render();

        _finalImageFramebuffer->bindForDrawing();

        _lightingRenderPass->render();
        _selectionRenderPass->render();
        _planeGridRenderPass->render();

        glActiveTexture(GL_TEXTURE0 + _finalImageRT->textureAddress.unit);
        glError::check();
        glBindTexture(GL_TEXTURE_2D_ARRAY, _finalImageRT->textureAddress.containerId);
        glError::check();
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
        glError::check();

        _finalImageFramebuffer->blitToDefault(_finalImageRT);
    }

    void renderer::update()
    {
        _gBufferRenderPass->update();
        _lightingRenderPass->update();
        _selectionRenderPass->update();
        _planeGridRenderPass->update();
    }
}