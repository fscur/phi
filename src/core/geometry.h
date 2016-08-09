#pragma once
#include <phi.h>
#include "coreApi.h"
#include "vertex.h"
#include "aabb.h"

namespace phi
{
    struct geometry
    {
    public:
        uint verticesCount;
        uint indicesCount;
        vertex* vboData;
        uint* eboData;
        uint vboSize;
        uint eboSize;
        aabb* aabb;

    public:
        CORE_API static geometry* createQuad(float size);
        CORE_API static geometry* createBox(float size);

        CORE_API ~geometry();

        CORE_API static geometry* create(vector<vertex> vertices, vector<uint> indices);
        CORE_API static geometry* create(
            uint verticesCount,
            float* positionsBuffer,
            float* texCoordsBuffer,
            float* normalsBuffer,
            uint indicesCount,
            uint* indicesBuffer);

        CORE_API static void calcNormals(vector<vertex>& vertices, vector<uint>& indices);
        CORE_API static void calcTangents(vector<vertex>& vertices, vector<uint>& indices);
        CORE_API static phi::aabb calcAabb(vector<vertex>& vertices);
    };
}