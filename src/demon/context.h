#pragma once
#include <phi.h>

#include "layer.h"

namespace demon
{
    class context
    {
    private:
        phi::vector<layer*> _layers;
    public:
        context(phi::vector<layer*>& layers);
        ~context();

        void update();
        void render();
    };
}