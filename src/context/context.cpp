#include <precompiled.h>
#include "context.h"

namespace phi
{
    using namespace phi;

    context::context(vector<layer*>& layers) :
        _layers(layers)
    {
        setLayersInputs();
    }

    context::context(vector<layer*>&& layers) :
        _layers(layers)
    {
        setLayersInputs();
    }

    context::~context()
    {
        for (auto& layer : _layers)
        {
            safeDelete(layer);
        }
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
