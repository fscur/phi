#ifndef _PHI_RENDERER_H_
#define _PHI_RENDERER_H_

#include "pipeline.h"

namespace phi
{
    class renderer
    {
    private:

        phi::pipeline* _pipeline;

    public:
        renderer(phi::pipeline* pipeline) :
            _pipeline(pipeline)
        {}

        ~renderer()
        {
            delete _pipeline;
        }

        RENDERING_API void render();
    };
}

#endif