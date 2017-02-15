#include <precompiled.h>
#include "textRenderPass.h"

#include <rendering\textureUnits.h>
#include <rendering\texturesManager.h>
#include <rendering\programBuilder.h>

namespace phi
{
    textRenderPass::textRenderPass(
        textRenderAdapter* renderAdapter, 
        const resolution& resolution, 
        const string& shadersPath) :
        renderPass(resolution)
    {
        _program = programBuilder::buildProgram(shadersPath, "text", "text");
        _program->addBuffer(renderAdapter->getGlyphRenderDataBuffer());
        _program->addBuffer(renderAdapter->getParentModelMatricesBuffer());
        
        _framebuffer = framebuffer::defaultFramebuffer;

        addVao(renderAdapter->getVao());
    }

    textRenderPass::~textRenderPass()
    {
    }

    void textRenderPass::onBeginRender()
    {
        _framebuffer->bindForDrawing();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendColor(1, 1, 1, 1);

        _program->bind();

        if (texturesManager::getIsBindless())
            _program->setUniform(0, texturesManager::textureArraysHandles);
        else
            _program->setUniform(0, texturesManager::textureArraysUnits);
    }

    void textRenderPass::onEndRender()
    {
        _program->unbind();

        glBlendColor(0, 0, 0, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }
}