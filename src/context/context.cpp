#include <precompiled.h>
#include "context.h"

#include <core\mesh.h>

#include "sceneId.h"

namespace phi
{
    using namespace phi;

    context::context(resolution& resolution, vector<layer*>& layers) :
        _layers(layers),
        _resolution(resolution)
    {
        setLayersInputs();
    }

    context::context(resolution& resolution, vector<layer*>&& layers) :
        _layers(layers),
        _resolution(resolution)
    {
        setLayersInputs();
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

            auto pickingFramebuffer = framebuffer::getPickingFramebuffer();
            auto pickingRenderTarget = framebuffer::getPickingRenderTarget();
            auto pixels = pickingFramebuffer->readPixels(pickingRenderTarget, x, y, 1, 1);

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
                    node->setIsSelected(true);
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
}
