#pragma once
#include <phi.h>

#include <rendering\gl.h>

#include "layer.h"

namespace demon
{
    class layerBuilder
    {
    public:
        static layer* buildScene(phi::gl* gl, float width, float height);
        static layer* buildUI(phi::gl* gl, float width, float height);
    };
}