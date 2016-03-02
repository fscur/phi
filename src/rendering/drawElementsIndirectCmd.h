#pragma once
#include <precompiled.h>

namespace phi
{
    struct drawElementsIndirectCmd
    {
        GLuint indicesCount;
        GLuint instanceCount;
        GLuint firstIndex;
        GLuint baseVertex;
        GLuint baseInstance;

        drawElementsIndirectCmd(
            GLuint indicesCount,
            GLuint instanceCount,
            GLuint firstIndex,
            GLuint baseVertex,
            GLuint baseInstance) :
            indicesCount(indicesCount),
            instanceCount(instanceCount),
            firstIndex(firstIndex),
            baseVertex(baseVertex),
            baseInstance(baseInstance)
        {
        }
    };
}