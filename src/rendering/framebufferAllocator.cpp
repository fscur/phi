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

    renderTarget* framebufferAllocator::getRenderTargetByName(string name) const
    {
        for (auto& pair : _renderTargets)
        {
            if (pair.first == name)
                return pair.second;
        }

        throw argumentException(name + " is not a valid render target name. RenderTarget not found.");
    }

    framebuffer * framebufferAllocator::getFramebufferByName(string name) const
    {
        for (auto& pair : _framebufferLayouts)
        {
            if (pair.second->name == name)
                return pair.first;
        }

        throw argumentException(name + " is not a valid framebuffer name. Framebuffer not found.");
    }

    framebuffer* phi::framebufferAllocator::newFramebuffer(const framebufferLayout* framebufferLayout, const resolution& resolution)
    {
        auto renderTargetsLayouts = framebufferLayout->getRenderTargetsLayouts();

        auto framebuffer = new phi::framebuffer();

        for (auto& pair : *renderTargetsLayouts)
        {
            auto attachment = pair.first;
            auto renderTargetLayout = pair.second;

            renderTarget* renderTarget;

            auto renderTargetName = renderTargetLayout->name;

            if (contains(_renderTargets, renderTargetName))
                renderTarget = _renderTargets[renderTargetName];
            else
            {
                auto textureLayout = renderTargetLayout->textureLayout;
                _layoutsCount[textureLayout]++;

                auto texture = new phi::texture(
                    static_cast<uint>(resolution.width),
                    static_cast<uint>(resolution.height),
                    textureLayout);

                renderTarget = new phi::renderTarget(renderTargetName, texture);
                _renderTargets[renderTargetName] = renderTarget;
                _renderTargetsLayouts[renderTarget] = textureLayout;
            }

            framebuffer->add(renderTarget, attachment);
        }

        _framebufferLayouts[framebuffer] = framebufferLayout;
        _framebuffers.push_back(framebuffer);
        return framebuffer;
    }

    void framebufferAllocator::allocate(const resolution& resolution)
    {
        for (auto& pair : _layoutsCount)
        {
            auto size = sizeui(static_cast<uint>(resolution.width), static_cast<uint>(resolution.height), _layoutsCount[pair.first]);
            texturesManager::reserveContainer(size, pair.first);
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
        for (auto& pair : _renderTargets)
        {
            auto renderTarget = pair.second;

            texturesManager::remove(renderTarget->texture);
        }

        for (auto& pair : _layoutsCount)
        {
            auto size = sizeui(static_cast<uint>(resolution.width), static_cast<uint>(resolution.height), _layoutsCount[pair.first]);
            texturesManager::reserveContainer(size, pair.first);
        }

        for (auto& pair : _renderTargets)
        {
            auto renderTarget = pair.second;
            auto layout = _renderTargetsLayouts[renderTarget];

            auto texture = new phi::texture(
                static_cast<uint>(resolution.width),
                static_cast<uint>(resolution.height),
                layout);

            texturesManager::add(texture);
            renderTarget->texture = texture;
        }

        for (auto& framebuffer : _framebuffers)
            framebuffer->attachRenderTargets();
    }
}