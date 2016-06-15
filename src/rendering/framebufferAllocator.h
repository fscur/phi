#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "framebuffer.h"

namespace phi
{
    class framebufferAllocator
    {
    private:
        vector<framebuffer*> _framebuffers;
        unordered_map<framebuffer*, const framebufferLayout*> _framebufferLayouts;
        unordered_map<textureLayout, uint> _layoutsCount;
        unordered_map<textureLayout, const renderTargetLayout*> _renderTargetsLayouts;
        unordered_map<const renderTargetLayout*, renderTarget*> _renderTargets;

    public:
        RENDERING_API framebufferAllocator();
        RENDERING_API ~framebufferAllocator();
        
        RENDERING_API framebuffer* newFramebuffer(const framebufferLayout* framebufferLayout, const resolution& resolution);

        RENDERING_API void allocate(const resolution& resolution);

        RENDERING_API void reallocate(const resolution& resolution);
    };
}