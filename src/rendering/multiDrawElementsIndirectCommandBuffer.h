#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\geometry.h>

#include "buffer.h"

namespace phi
{
    class multiDrawElementsIndirectCommandBuffer
    {
    private:
        struct geometryInstance
        {
            geometryInstance(const geometry* geometry) :
                geometry(geometry),
                instanceCount(1)
            {
            }

            const geometry* geometry;
            uint instanceCount;
        };

    private:
        GLuint indicesOffset;
        GLuint verticesOffset;
        GLuint instancesOffset;

        vector<geometryInstance*> _geometries;
        map<const geometry*, geometryInstance*> _geometriesInstances;

        buffer* _drawCmdBuffer;

    public:
        RENDERING_API multiDrawElementsIndirectCommandBuffer();
        RENDERING_API ~multiDrawElementsIndirectCommandBuffer();

        RENDERING_API void add(const geometry* geometry);
        RENDERING_API void create();
        RENDERING_API void bind();
        RENDERING_API void unbind();
    };
}