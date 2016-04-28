#include <precompiled.h>
#include "gBufferRenderPass.h"

namespace phi
{
    gBufferRenderPass::gBufferRenderPass(gl* gl, float w, float h) :
        _gl(gl),
        _shader(nullptr),
        _framebuffer(new framebuffer()),
        _targets(vector<renderTarget*>()),
        _batches(vector<batch*>()),
        _w(w),
        _h(h)
    {
        initShader();
        initRenderTargets();
    }

    gBufferRenderPass::~gBufferRenderPass()
    {
        for (auto target : _targets)
            safeDelete(target);

        safeDelete(_framebuffer);
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

        _shader = _gl->shadersManager->load("geometryPass", attribs);
        _shader->addUniform(0, "textureArrays");
    }

    void gBufferRenderPass::initRenderTargets()
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
            auto renderTarget = new phi::renderTarget(
                attachment, 
                static_cast<GLint>(_w),
                static_cast<GLint>(_h),
                textureAddress,
                texture);

            _targets.push_back(renderTarget);
            gBufferRenderPass::_framebuffer->add(renderTarget);
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
        _shader->bind();

        if (_gl->currentState.useBindlessTextures)
            _shader->setUniform(0, _gl->texturesManager->handles);
        else
            _shader->setUniform(0, _gl->texturesManager->units);

        _shader->unbind();
    }

    void gBufferRenderPass::render()
    {
        _framebuffer->bindForDrawing();

        glDepthMask(GL_TRUE);
        glError::check();
        glEnable(GL_DEPTH_TEST);
        glError::check();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glError::check();

        _shader->bind();

        for (auto batch : _batches)
            batch->render();

        _shader->unbind();

        _framebuffer->unbind(GL_FRAMEBUFFER);
        _framebuffer->bindForReading();

        auto w = static_cast<GLint>(_w);
        auto h = static_cast<GLint>(_h);

        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glError::check();
    }
}