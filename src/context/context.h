#pragma once
#include <phi.h>
#include "contextApi.h"

#include <core\resolution.h>
#include <core\keyboardEventArgs.h>
#include <core\mouseEventArgs.h>

#include <rendering\framebufferAllocator.h>

#include "layer.h"

namespace phi
{
    class context
    {
    private:
        vector<layer*> _layers;
        framebufferAllocator* _framebufferAllocator;
        resolution _resolution;
        framebuffer* _pickingFramebuffer;
        renderTarget* _pickingRenderTarget;

    private:
        void setLayersInputs();
        void initialize();

    public:
        CONTEXT_API context(resolution& resolution, framebufferAllocator* framebufferAllocator, vector<layer*>& layers);
        CONTEXT_API context(resolution& resolution, framebufferAllocator* framebufferAllocator, vector<layer*>&& layers);
        CONTEXT_API ~context();


        CONTEXT_API void update();
        CONTEXT_API void render();

        CONTEXT_API void onMouseDown(mouseEventArgs* e);
        CONTEXT_API void onMouseMove(mouseEventArgs* e);
        CONTEXT_API void onMouseUp(mouseEventArgs* e);
        CONTEXT_API void onMouseWheel(mouseEventArgs* e);
        CONTEXT_API void onKeyDown(keyboardEventArgs* e);
        CONTEXT_API void onKeyUp(keyboardEventArgs* e);

        CONTEXT_API void resize(const resolution& resolution);
    };
}