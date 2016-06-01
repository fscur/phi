#pragma once
#include <phi.h>
#include "contextApi.h"

#include "layer.h"

namespace phi
{
    class context
    {
    private:
        phi::vector<layer*> _layers;
    public:
        CONTEXT_API context(phi::vector<layer*>& layers);
        CONTEXT_API context(phi::vector<layer*>&& layers);
        CONTEXT_API ~context();

        CONTEXT_API void update();
        CONTEXT_API void render();
    };
}