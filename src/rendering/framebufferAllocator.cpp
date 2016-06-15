#include <precompiled.h>
#include "framebufferAllocator.h"
#include <rendering\texturesManager.h>

namespace phi
{
    framebufferAllocator::framebufferAllocator()
    {

    }

    framebufferAllocator::~framebufferAllocator()
    {
    }

    framebuffer* phi::framebufferAllocator::newFramebuffer(const framebufferLayout* framebufferLayout, const resolution& resolution)
    {
        auto renderTargetsLayouts = framebufferLayout->getRenderTargetsLayouts();

        auto framebuffer = new phi::framebuffer();

        for (auto& pair : *renderTargetsLayouts)
        {
            auto attachment = pair.first;
            auto renderTargetLayout = pair.second;
            auto textureLayout = renderTargetLayout->textureLayout;

            if (!contains(_renderTargetsLayouts, textureLayout))
                _renderTargetsLayouts[textureLayout] = renderTargetLayout;
            
            _layoutsCount[textureLayout]++;

            auto texture = new phi::texture(
                static_cast<uint>(resolution.width),
                static_cast<uint>(resolution.height),
                textureLayout);

            renderTarget* renderTarget;

            if (contains(_renderTargets, renderTargetLayout))
                renderTarget = _renderTargets[renderTargetLayout];
            else
                renderTarget = new phi::renderTarget(renderTargetLayout->name, texture);

            framebuffer->add(renderTarget, attachment);

            _renderTargets[renderTargetLayout] = renderTarget;
        }

        _framebufferLayouts[framebuffer] = framebufferLayout;
        _framebuffers.push_back(framebuffer);
        return framebuffer;
    }

    void framebufferAllocator::allocate(const resolution& resolution)
    {
        for (auto& pair : _renderTargetsLayouts)
        {
            auto size = sizeui(static_cast<uint>(resolution.width), static_cast<uint>(resolution.height), _layoutsCount[pair.first]);
            texturesManager::reserveContainer(size, pair.second->textureLayout);
        }

        for (auto& pair : _renderTargets)
        {
            auto renderTarget = pair.second;
            texturesManager::add(renderTarget->texture);
        }

        for (auto& framebuffer : _framebuffers)
            framebuffer->attachRenderTargets();
    }

    void framebufferAllocator::reallocate(const resolution& resolution)
    {
        //for (auto& framebuffer : _framebuffers)
        //{
        //    if (_isDefaultFramebuffer)
        //        return;

        //    auto i = 0;

        //    /*glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //    glDeleteFramebuffers(1, &_id);
        //    glCreateFramebuffers(1, &_id);*/

        //    for (auto& renderTarget : _renderTargets)
        //    {
        //        auto attachment = _renderTargetsAttachments[renderTarget];

        //        auto texture = new phi::texture(
        //            static_cast<uint>(resolution.width),
        //            static_cast<uint>(resolution.height),
        //            renderTarget->layout);

        //        auto textureAddress = texturesManager::add(texture);

        //        renderTarget->w = static_cast<GLint>(resolution.width);
        //        renderTarget->h = static_cast<GLint>(resolution.height);
        //        renderTarget->textureAddress = textureAddress;

        //        glBindFramebuffer(GL_FRAMEBUFFER, _id);

        //        glNamedFramebufferTextureLayer(
        //            _id,
        //            attachment,
        //            textureAddress.containerId,
        //            0,
        //            static_cast<GLint>(textureAddress.page));

        //        auto status = glCheckNamedFramebufferStatus(_id, GL_FRAMEBUFFER);

        //        switch (status)
        //        {
        //        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        //            phi::debug("incomplete attachment");
        //            break;
        //        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        //            phi::debug("incomplete draw buffer");
        //            break;
        //        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        //            phi::debug("incomplete layer targets");
        //            break;
        //        default:
        //            break;
        //        }
        //    }
        //}
    }
}