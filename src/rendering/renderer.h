#ifndef _PHI_RENDERER_H_
#define _PHI_RENDERER_H_

#include "renderingSystem.h"

namespace phi
{
    class renderer
    {
    private:
        pipeline* _pipeline;

    public:
        RENDERING_API renderer(phi::pipeline* pipeline) :
            _pipeline(pipeline)
        {
        }

        RENDERING_API ~renderer() {}
        RENDERING_API void render();
    };
}

#endif