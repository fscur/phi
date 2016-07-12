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

    inline void framebufferLayout::addRenderTargetLayout(const GLenum attachment, const renderTargetLayout * renderTargetLayout)
    {
        _renderTargetsLayouts->insert_or_assign(attachment, renderTargetLayout);
    }

    inline void framebufferLayout::addRenderTarget(const GLenum attachment, renderTarget* renderTarget)
    {
        _renderTargets->insert_or_assign(attachment, renderTarget);
    }
}