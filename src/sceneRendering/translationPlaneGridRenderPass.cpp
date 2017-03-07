#include <precompiled.h>
#include "translationPlaneGridRenderPass.h"

#include <core\time.h>
#include <core\geometry.h>

#include <rendering\textureUnits.h>
#include <rendering\texturesManager.h>
#include <rendering\gBufferRTAddresses.h>
#include <rendering\textureAddress.h>
#include <rendering\vertexArrayObject.h>

#include <rendering\programBuilder.h>
#include <rendering\framebufferLayoutBuilder.h>
#include <rendering\vertexBuffer.h>
#include <rendering\indexBuffer.h>

namespace phi
{
    translationPlaneGridRenderPass::translationPlaneGridRenderPass(
        translationPlaneGridRenderAdapter* renderAdapter, 
        const resolution& resolution, 
        const string& shadersPath, 
        framebufferAllocator* framebufferAllocator) :
        renderPass(resolution)
    {
        _framebuffer = framebufferAllocator->getFramebuffer("defaultFramebuffer");

        _program = programBuilder::buildProgram(shadersPath, "planeGrid", "planeGrid");
        _program->addBuffer(renderAdapter->getMousePlaneGridRenderDataBuffer());
        
        addVao(renderAdapter->getVao());
    }

    translationPlaneGridRenderPass::~translationPlaneGridRenderPass()
    {
    }

    void translationPlaneGridRenderPass::onBeginRender()
    {
        _framebuffer->bindForDrawing();
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0f, 0.0f);
        //glEnable(GL_CLIP_DISTANCE0);
        //glEnable(GL_CLIP_DISTANCE1);
        //glEnable(GL_CLIP_DISTANCE2);

        _program->bind();
    }

    void translationPlaneGridRenderPass::onEndRender()
    {
        _program->unbind();

        //glDisable(GL_CLIP_DISTANCE2);
        //glDisable(GL_CLIP_DISTANCE1);
        //glDisable(GL_CLIP_DISTANCE0);

        glPolygonOffset(0.0f, 0.0f);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glEnable(GL_CULL_FACE);
    }
}