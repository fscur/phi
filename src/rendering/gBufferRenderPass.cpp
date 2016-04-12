#include <precompiled.h>
#include "gBufferRenderPass.h"

namespace phi
{
    gBufferRenderPass::gBufferRenderPass(gl* gl, size_t w, size_t h) :
        _gl(gl),
        _w(w),
        _h(h),
        shader(nullptr),
        framebuffer(new phi::framebuffer()),
        targets(vector<renderTarget*>()),
        batches(vector<batch*>())
    {
        initShader();
        initRenderTargets();
    }

    gBufferRenderPass::~gBufferRenderPass()
    {
        for (auto target : targets)
            safeDelete(target);

        safeDelete(framebuffer);
    }

    void gBufferRenderPass::initShader()
    {
        vector<string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");
        attribs.push_back("inMaterialId");
        attribs.push_back("inModelMatrix");

        shader = _gl->shadersManager->load("geometryPass", attribs);
        shader->addUniform(0, "textureArrays");
    }

    void gBufferRenderPass::initRenderTargets()
    {
        auto reserveContainer = [&](GLenum internalFormat, size_t size)
        {
            auto layout = textureContainerLayout();
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
            auto renderTarget = new phi::renderTarget(
                attachment, 
                static_cast<GLint>(_w),
                static_cast<GLint>(_h),
                textureAddress,
                texture);

            targets.push_back(renderTarget);
            gBufferRenderPass::framebuffer->add(renderTarget);
        };

        reserveContainer(GL_RGBA16F, 3);
        reserveContainer(GL_DEPTH_COMPONENT32, 1);

        createRenderTarget(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA);
        createRenderTarget(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA);
        createRenderTarget(GL_COLOR_ATTACHMENT2, GL_RGBA16F, GL_RGBA);
        createRenderTarget(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);
    }

    void gBufferRenderPass::update()
    {
        shader->bind();

        if (_gl->currentState.useBindlessTextures)
            shader->setUniform(0, _gl->texturesManager->handles);
        else
            shader->setUniform(0, _gl->texturesManager->units);

        shader->unbind();
    }

    void gBufferRenderPass::render()
    {
        framebuffer->bindForDrawing();

        glDepthMask(GL_TRUE);
        glError::check();
        glEnable(GL_DEPTH_TEST);
        glError::check();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glError::check();

        shader->bind();

        for (auto batch : batches)
            batch->render();

        shader->unbind();

        framebuffer->unbind(GL_FRAMEBUFFER);
        framebuffer->bindForReading();

        auto w = static_cast<GLint>(_w);
        auto h = static_cast<GLint>(_h);

        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glError::check();
    }
}