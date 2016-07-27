#include <precompiled.h>
#include "context.h"

#include <core\mesh.h>

#include "pickingId.h"

namespace phi
{
    using namespace phi;

    context::context(
        resolution& resolution,
        framebufferAllocator* framebufferAllocator,
        commandsManager* commandsManager,
        vector<layer*>&& layers) :
        _layers(layers),
        _framebufferAllocator(framebufferAllocator),
        _resolution(resolution),
        _pickingFramebuffer(nullptr),
        _pickingRenderTarget(nullptr),
        _commandsManager(commandsManager)
    {
        _pickingFramebuffer = framebufferAllocator->getFramebuffer("pickingFramebuffer");
        _pickingRenderTarget = _pickingFramebuffer->getRenderTarget("pickingRenderTarget");

        initialize(); //TODO:is this the right place?
    }

    context::~context()
    {
    }

    void context::initialize()
    {
        for (auto& layer : _layers)
            layer->initialize();
    }

    void context::update()
    {
        for (auto& layer : _layers)
            layer->update();
    }

    void context::render()
    {
        for (auto& layer : _layers)
            layer->render();
    }

    void context::onMouseDown(mouseEventArgs* e)
    {
        for (auto& layer : _layers)
            layer->onMouseDown(e);
    }

    void context::onMouseMove(mouseEventArgs* e)
    {
        for (auto& layer : _layers)
            layer->onMouseMove(e);
    }

    void context::onMouseUp(mouseEventArgs* e)
    {
        for (auto& layer : _layers)
            layer->onMouseUp(e);
    }

    void context::onBeginMouseWheel(mouseEventArgs* e)
    {
        for (auto& layer : _layers)
            layer->onBeginMouseWheel(e);
    }

    void context::onMouseWheel(mouseEventArgs* e)
    {
        for (auto& layer : _layers)
            layer->onMouseWheel(e);
    }

    void context::onEndMouseWheel(mouseEventArgs* e)
    {
        for (auto& layer : _layers)
            layer->onEndMouseWheel(e);
    }

    void context::onKeyDown(keyboardEventArgs* e)
    {
        for (auto& layer : _layers)
            layer->onKeyDown(e);
    }

    void context::onKeyUp(keyboardEventArgs* e)
    {
        for (auto& layer : _layers)
            layer->onKeyUp(e);
    }

    void context::resize(const resolution& resolution)
    {
        for (auto& layer : _layers)
            layer->resize(resolution);
    }
}
