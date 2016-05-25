#include <precompiled.h>
#include "context.h"

namespace demon
{
    using namespace phi;

    context::context(vector<layer*>& layers) :
        _layers(layers)
    {
    }

    context::~context()
    {
        for (auto& layer : _layers)
        {
            safeDelete(layer);
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
}
