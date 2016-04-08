#pragma once
#include <phi.h>
#include "coreApi.h"
#include "vertex.h"

namespace phi
{
    struct CORE_API geometry
    {
    private:
        static geometry* _quad;

    public:
        uint verticesCount;
        uint indicesCount;
        vertex* vboData;
        uint* eboData;
        uint vboSize;
        uint eboSize;

    public:
        static geometry* quad();

    public:
        geometry();
        ~geometry();

        static geometry* create(vector<vertex> vertices, vector<uint> indices);
        static geometry* create(
            uint verticesCount,
            float* positionsBuffer,
            float* texCoordsBuffer,
            float* normalsBuffer,
            uint indicesCount,
            uint* indicesBuffer);

        static void calcNormals(vector<vertex>& vertices, vector<uint>& indices);
        static void calcTangents(vector<vertex>& vertices, vector<uint>& indices);
    };
}