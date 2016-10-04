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
        for (auto& item : _framebuffers)
            safeDelete(item);

        for (auto& pair : _renderTargets)
            safeDelete(pair.second);
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
        for (auto& item : _framebuffers)
        {
            if (item->getName() == name)
                return item;
        }

        throw argumentException(name + " is not a valid framebuffer name. Framebuffer not found.");
    }

    void framebufferAllocator::addRenderTargetLayouts(
        framebuffer* framebuffer,
        const unordered_map<GLenum, const renderTargetLayout*>* renderTargetsLayouts,
        const resolution & resolution)
    {
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
    }

    void framebufferAllocator::addRenderTargets(
        framebuffer* framebuffer,
        const unordered_map<GLenum, renderTarget*>* renderTargets,
        const resolution & resolution)
    {
        for (auto& pair : *renderTargets)
        {
            auto attachment = pair.first;
            auto renderTarget = pair.second;

            auto renderTargetName = renderTarget->name;

            if (contains(_renderTargets, renderTargetName))
                renderTarget = _renderTargets[renderTargetName];
            else
            {
                auto textureLayout = renderTarget->texture->layout;
                _layoutsCount[textureLayout]++;

                _renderTargets[renderTargetName] = renderTarget;
                _renderTargetsLayouts[renderTarget] = textureLayout;
            }

            framebuffer->add(renderTarget, attachment);
        }
    }

    framebuffer* framebufferAllocator::newFramebuffer(framebufferLayout* framebufferLayout, const resolution& resolution)
    {
        auto framebuffer = new phi::framebuffer(framebufferLayout->getName());
        auto renderTargetsLayouts = framebufferLayout->getRenderTargetsLayouts();
        addRenderTargetLayouts(framebuffer, renderTargetsLayouts, resolution);

        auto renderTargets = framebufferLayout->getRenderTargets();
        addRenderTargets(framebuffer, renderTargets, resolution);

        _framebuffers.push_back(framebuffer);
        return framebuffer;
    }

    void framebufferAllocator::allocate(const resolution& resolution)
    {
        for (auto& pair : _layoutsCount)
        {
            auto size = sizeui(static_cast<uint>(resolution.width), static_cast<uint>(resolution.height), _layoutsCount[pair.first]);
            texturesManager::reserveTextureArray(size, pair.first);
        }

        for (auto& pair : _renderTargets)
        {
            auto renderTarget = pair.second;
            texturesManager::addTexture(renderTarget->texture);
        }

        for (auto& framebuffer : _framebuffers)
            framebuffer->attachRenderTargets();
    }

    void framebufferAllocator::reallocate(const resolution& resolution)
    {
        for (auto& pair : _renderTargets)
        {
            auto renderTarget = pair.second;

            texturesManager::removeTexture(renderTarget->texture);
        }

        for (auto& pair : _layoutsCount)
        {
            auto layout = pair.first;

            if (layout.levels > 1)
                layout.levels = texturesManager::getMaxLevels(static_cast<uint>(resolution.width), static_cast<uint>(resolution.height));

            auto size = sizeui(static_cast<uint>(resolution.width), static_cast<uint>(resolution.height), _layoutsCount[pair.first]);

            texturesManager::reserveTextureArray(size, layout);
        }

        for (auto& pair : _renderTargets)
        {
            auto renderTarget = pair.second;
            auto layout = _renderTargetsLayouts[renderTarget];
            
            if (layout.levels > 1)
                layout.levels = texturesManager::getMaxLevels(static_cast<uint>(resolution.width), static_cast<uint>(resolution.height));

            auto texture = new phi::texture(
                static_cast<uint>(resolution.width),
                static_cast<uint>(resolution.height),
                layout);

            texturesManager::addTexture(texture);
            renderTarget->texture = texture;
        }

        for (auto& framebuffer : _framebuffers)
            framebuffer->attachRenderTargets();
    }

    inline renderTarget* framebufferAllocator::getRenderTarget(string renderTargetName) const { return getRenderTargetByName(renderTargetName); }
    inline framebuffer* framebufferAllocator::getFramebuffer(string framebufferName) const { return getFramebufferByName(framebufferName); }
}