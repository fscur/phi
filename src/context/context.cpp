#include <precompiled.h>
#include "context.h"

#include <core\mesh.h>

#include "sceneId.h"

namespace phi
{
    using namespace phi;

    context::context(resolution& resolution, framebufferAllocator* framebufferAllocator, vector<layer*>& layers) :
        _layers(layers),
        _framebufferAllocator(framebufferAllocator),
        _resolution(resolution)
    {
        _pickingFramebuffer = framebufferAllocator->getFramebuffer("pickingFramebuffer");
        _pickingRenderTarget = _pickingFramebuffer->getRenderTarget("pickingRenderTarget");

        setLayersInputs();
        initialize(); //TODO:is it the right place?

    }

    context::context(resolution& resolution, framebufferAllocator* framebufferAllocator, vector<layer*>&& layers) :
        _layers(layers),
        _framebufferAllocator(framebufferAllocator),
        _resolution(resolution)
    {
        _pickingFramebuffer = framebufferAllocator->getFramebuffer("pickingFramebuffer");
        _pickingRenderTarget = _pickingFramebuffer->getRenderTarget("pickingRenderTarget");

        setLayersInputs();
        initialize(); //TODO:is it the right place?
    }

    context::~context()
    {
    }

    void context::setLayersInputs()
    {
        layer* previous = nullptr;
        for (auto& layer : _layers)
        {
            if (previous != nullptr)
                layer->onInputChanged.raise(previous);

            previous = layer;
        }
    }

    void context::initialize()
    {
        for (auto& layer : _layers)
            layer->initialize();
    }

    void context::update()
    {
        for (auto& layer : _layers)
        {
            layer->update();
        }
    }

    void context::render()
    {
        for (auto& layer : _layers)
        {
            layer->render();
        }
    }

    void context::onMouseDown(mouseEventArgs* e)
    {
        if (e->leftButtonPressed)
        {
            auto x = static_cast<GLint>(e->x);
            auto y = static_cast<GLint>(_resolution.height - e->y);

            auto pixels = _pickingFramebuffer->readPixels(_pickingRenderTarget, x, y, 1, 1);

            auto r = static_cast<int>(pixels.r);
            auto g = static_cast<int>(pixels.g) << 8;
            auto b = static_cast<int>(pixels.b) << 16;

            auto id = r | g | b;

            mesh* mesh = nullptr;
            if (id)
            {
                mesh = sceneId::getMesh(id);
                if (mesh)
                {
                    auto node = mesh->getNode();
                    debug("descomentar o pick");
                    //node->setIsSelected(true);
                }
            }
        }

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

    void context::onMouseWheel(mouseEventArgs* e)
    {
        for (auto& layer : _layers)
            layer->onMouseWheel(e);
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
