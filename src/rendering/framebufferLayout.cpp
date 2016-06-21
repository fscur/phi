#include <precompiled.h>
#include "framebufferLayout.h"

namespace phi
{
    framebufferLayout::framebufferLayout(string name) :
        name(name),
        _renderTargetsLayouts(new unordered_map<GLenum, const renderTargetLayout*>()),
        _renderTargets(new unordered_map<GLenum, renderTarget*>())
    {
    }

    inline void framebufferLayout::addRenderTargetLayout(GLenum attachment, const renderTargetLayout * renderTargetLayout)
    {
        _renderTargetsLayouts->insert_or_assign(attachment, renderTargetLayout);
    }

    inline void framebufferLayout::addRenderTarget(GLenum attachment, renderTarget * renderTarget)
    {
        _renderTargets->insert_or_assign(attachment, renderTarget);
    }
}