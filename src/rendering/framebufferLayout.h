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
        string _name;
        unordered_map<GLenum, const renderTargetLayout*>* _renderTargetsLayouts;
        unordered_map<GLenum, renderTarget*>* _renderTargets;

    public:
        RENDERING_API framebufferLayout(const string& name);
        RENDERING_API framebufferLayout(const framebufferLayout& original);
        RENDERING_API ~framebufferLayout();

        RENDERING_API void addRenderTargetLayout(GLenum attachment, const renderTargetLayout* renderTargetLayout);
        RENDERING_API void addRenderTarget(GLenum attachment, renderTarget* renderTarget);

        string getName() const { return _name; }
        unordered_map<GLenum, const renderTargetLayout*>* getRenderTargetsLayouts() const { return _renderTargetsLayouts; }
        unordered_map<GLenum, renderTarget*>* getRenderTargets() const { return _renderTargets; }
    };
}