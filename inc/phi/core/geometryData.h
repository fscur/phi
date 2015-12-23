#ifndef _PHI_GEOMETRY_DATA_H_
#define _PHI_GEOMETRY_DATA_H_

#include "vertex.h"

#include <string>
#include <list>
#include <vector>

namespace phi
{
    class geometryData
    {
    private:
        uint _verticesCount;
        uint _indicesCount;

        float* _positionsBuffer;
        float* _texCoordsBuffer;
        float* _normalsBuffer;
        float* _tangentsBuffer;
        uint* _indicesBuffer;

        uint _pSize;
        uint _tSize;
        uint _nSize;
        uint _tgSize;
        uint _iSize;

        std::vector<vertex> _vertices;
        std::vector<uint>* _indices;

    private:

        static void createBuffers(
            std::vector<vertex> vertices,
            float* &positionsBuffer,
            float* &texCoordsBuffer,
            float* &normalsBuffer,
            float* &tangentsBuffer);

    protected:
        CORE_API geometryData();
        CORE_API void addVertices(std::vector<vertex> vertices, std::vector<uint>* indices);

    public:
        CORE_API ~geometryData();

        CORE_API static geometryData* create(std::vector<vertex> &vertices, std::vector<uint>* indices);
        CORE_API static geometryData* create(
            uint verticesCount,
            float* positionsBuffer,
            float* texCoordsBuffer,
            float* normalsBuffer,
            uint indicesCount,
            uint* indicesBuffer);

        CORE_API inline std::vector<vertex> getVertices() const { return _vertices; }
        CORE_API inline std::vector<uint>* getIndices() const { return _indices; }
        CORE_API inline uint getVerticesCount() const { return _verticesCount; }
        CORE_API inline uint getIndicesCount() const { return _indicesCount; }
        CORE_API inline float* getPositionsBuffer() const { return _positionsBuffer; }
        CORE_API inline float* getTexCoordsBuffer() const { return _texCoordsBuffer; }
        CORE_API inline float* getNormalsBuffer() const { return _normalsBuffer; }
        CORE_API inline float* getTangentsBuffer() const { return _tangentsBuffer; }
        CORE_API inline uint* getIndicesBuffer() const { return _indicesBuffer; }
        CORE_API inline uint getPSize() const { return _pSize; }
        CORE_API inline uint getTSize() const { return _tSize; }
        CORE_API inline uint getNSize() const { return _nSize; }
        CORE_API inline uint getTGSize() const { return _tgSize; }
        CORE_API inline uint getISize() const { return _iSize; }

        CORE_API static void calcNormals(std::vector<vertex> &vertices, std::vector<uint> &indices);
        CORE_API static void calcTangents(std::vector<vertex> &vertices, std::vector<uint> &indices);
    };
}
#endif