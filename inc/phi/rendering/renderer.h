#ifndef _PHI_RENDERER_H_
#define _PHI_RENDERER_H_

#include <phi\core\geometry.h>
#include "bufferLockManager.h"
#include "drawElementsIndirectCmd.h"
#include "shader.h"
#include "shaderManager.h"

#include "buffer.h"
#include "vertexBuffer.h"
#include "elementBuffer.h"
#include "mesh.h"
#include "material.h"
#include "materialGpuData.h"
#include "pipeline.h"

#include <vector>
#include <map>

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