#pragma once

#include <phi.h>
#include "renderingApi.h"
#include "renderTarget.h"
#include "renderTargetLayout.h"

namespace phi
{
    class framebufferLayout
    {
    private:
        unordered_map<GLenum, const renderTargetLayout*>* _renderTargetsLayouts;
        unordered_map<GLenum, renderTarget*>* _renderTargets;

    public:
        string name;

    public:
        RENDERING_API framebufferLayout(string name);
        RENDERING_API void addRenderTargetLayout(GLenum attachment, const renderTargetLayout* renderTargetLayout);
        RENDERING_API void addRenderTarget(GLenum attachment, renderTarget* renderTarget);

        unordered_map<GLenum, const renderTargetLayout*>* getRenderTargetsLayouts() const { return _renderTargetsLayouts; }
        unordered_map<GLenum, renderTarget*>* getRenderTargets() const { return _renderTargets; }
    };
}