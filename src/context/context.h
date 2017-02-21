#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\resolution.h>
#include <input\keyboardEventArgs.h>
#include <input\mouseEventArgs.h>

#include <rendering\framebufferAllocator.h>

#include <application\commandsManager.h>

#include <layers/layer.h>

namespace phi
{
    class context
    {
    public:
        CONTEXT_API context(
            resolution& resolution,
            framebufferAllocator* framebufferAllocator,
            commandsManager* commandsManager,
            vector<layer*>&& layers);

        CONTEXT_API ~context();

        CONTEXT_API void initialize();
        CONTEXT_API void update();
        CONTEXT_API void render();

        CONTEXT_API void onMouseDown(mouseEventArgs* e);
        CONTEXT_API void onMouseUp(mouseEventArgs* e);
        CONTEXT_API void onMouseClick(mouseEventArgs* e);
        CONTEXT_API void onMouseDoubleClick(mouseEventArgs* e);
        CONTEXT_API void onMouseMove(mouseEventArgs* e);
        CONTEXT_API void onBeginMouseWheel(mouseEventArgs* e);
        CONTEXT_API void onMouseWheel(mouseEventArgs* e);
        CONTEXT_API void onEndMouseWheel(mouseEventArgs* e);
        CONTEXT_API void onKeyDown(keyboardEventArgs* e);
        CONTEXT_API void onKeyUp(keyboardEventArgs* e);

        CONTEXT_API void resize(const resolution& resolution);

    private:
        vector<layer*> _layers;
        framebufferAllocator* _framebufferAllocator;
        resolution _resolution;
        framebuffer* _pickingFramebuffer;
        renderTarget* _pickingRenderTarget;

        commandsManager* _commandsManager;
    };
}