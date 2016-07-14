#include <precompiled.h>
#include "framebufferLayout.h"

namespace phi
{
    framebufferLayout::framebufferLayout(const string& name) :
        _name(name),
        _renderTargetsLayouts(new unordered_map<GLenum, const renderTargetLayout*>()),
        _renderTargets(new unordered_map<GLenum, renderTarget*>())
    {
    }

    framebufferLayout::~framebufferLayout()
    {
        for (auto& pair : *_renderTargetsLayouts)
            safeDelete(pair.second);

        safeDelete(_renderTargetsLayouts);
        safeDelete(_renderTargets);
    }

    inline void framebufferLayout::addRenderTargetLayout(GLenum attachment, const renderTargetLayout * renderTargetLayout)
    {
        (*_renderTargetsLayouts)[attachment] = renderTargetLayout;
    }

    inline void framebufferLayout::addRenderTarget(GLenum attachment, renderTarget* renderTarget)
    {
        (*_renderTargets)[attachment] = renderTarget;
    }
}