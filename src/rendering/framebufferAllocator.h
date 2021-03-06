#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "framebuffer.h"
#include "framebufferLayout.h"
#include "renderTargetLayout.h"

namespace phi
{
    class framebufferAllocator
    {
    private:
        unordered_map<textureLayout, uint> _layoutsCount;
        vector<framebuffer*> _framebuffers;
        unordered_map<string, renderTarget*> _renderTargets;
        unordered_map<renderTarget*, textureLayout> _renderTargetsLayouts;

    private:
        renderTarget* getRenderTargetByName(string name) const;
        framebuffer* getFramebufferByName(string name) const;

        void addRenderTargetLayouts(
            framebuffer* framebuffer,
            const unordered_map<GLenum, const renderTargetLayout*>* renderTargetsLayouts,
            const resolution & resolution);

        void addRenderTargets(
            framebuffer* framebuffer,
            const unordered_map<GLenum, renderTarget*>* renderTargets);

    public:
        RENDERING_API framebufferAllocator();
        RENDERING_API ~framebufferAllocator();
        
        RENDERING_API framebuffer* newFramebuffer(framebufferLayout* framebufferLayout, const resolution& resolution);
        RENDERING_API void allocate(const resolution& resolution);
        RENDERING_API void reallocate(const resolution& resolution);
        RENDERING_API renderTarget* getRenderTarget(string renderTargetName) const;
        RENDERING_API framebuffer* getFramebuffer(string framebufferName) const;
    };
}