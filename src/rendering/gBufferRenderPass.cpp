#include <precompiled.h>
#include "gBufferRenderPass.h"

namespace phi
{
    gBufferRenderPass::gBufferRenderPass(phi::gl* gl, size_t w, size_t h) :
        _w(w),
        _h(h),
        _gl(gl),
        framebuffer(new phi::framebuffer())
    {
        initShader();
        initRenderTargets();
    }

    gBufferRenderPass::~gBufferRenderPass()
    {}

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

        auto createTexture = [&](
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

            auto rt = new phi::renderTarget(
                attachment, 
                static_cast<GLint>(_w), 
                static_cast<GLint>(_h), 
                _gl->texturesManager->add(texture));

            targets.push_back(rt);
            gBufferRenderPass::framebuffer->add(rt);
        };

        reserveContainer(GL_RGBA16F, 3);
        reserveContainer(GL_DEPTH_COMPONENT32, 1);
        createTexture(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA);
        createTexture(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA);
        createTexture(GL_COLOR_ATTACHMENT2, GL_RGBA16F, GL_RGBA);
        createTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);
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
        framebuffer->bindForReading(targets[3]);

        auto w = static_cast<GLint>(_w);
        auto h = static_cast<GLint>(_h);

        glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glError::check();
    }
}