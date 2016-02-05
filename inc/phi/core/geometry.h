#ifndef _PHI_GEOMETRY_H_
#define _PHI_GEOMETRY_H_

#include "core.h"

#include "vertex.h"

#include <string>
#include <list>
#include <vector>

namespace phi
{
    struct geometry
    {
    private:
        static geometry* _quad;

    public:
        size_t verticesCount;
        size_t indicesCount;
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
#endif