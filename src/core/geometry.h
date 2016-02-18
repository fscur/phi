#pragma once

#include "vertex.h"

namespace phi
{
    struct geometry
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
        CORE_API static geometry* quad();

    public:
        CORE_API geometry::geometry() {}

    public:
        CORE_API ~geometry()
        {
            delete[] vboData;
            delete[] eboData;
        }

        CORE_API static geometry* create(std::vector<vertex> vertices, std::vector<uint> indices);
        CORE_API static geometry* create(
            uint verticesCount,
            float* positionsBuffer,
            float* texCoordsBuffer,
            float* normalsBuffer,
            uint indicesCount,
            uint* indicesBuffer);

        CORE_API static void calcNormals(std::vector<vertex> &vertices, std::vector<uint> &indices);
        CORE_API static void calcTangents(std::vector<vertex> &vertices, std::vector<uint> &indices);
    };
}