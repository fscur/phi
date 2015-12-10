#ifndef _PHI_GEOMETRY_DATA_H_
#define _PHI_GEOMETRY_DATA_H_

#include "loader.h"

namespace phi
{
    class geometryData
    {
    private:
        uint _verticesSize;
        float* _positions;
        float* _textureCoords;
        float* _normals;
        uint _indicesSize;
        uint* _indices;
        char* _materialName;

    public:
        geometryData(uint verticesSize, float* positions, float* textureCoords, float* normals, uint indicesSize, uint* indices, char* materialName)
        {
            _verticesSize = verticesSize;
            _positions = positions;
            _textureCoords = textureCoords;
            _normals = normals;
            _indicesSize = indicesSize;
            _indices = indices;
            _materialName = materialName;
        }

        LOADER_API uint getVerticesSize() { return _verticesSize; }
        LOADER_API float* getPositions() { return _positions; }
        LOADER_API float* getTextureCoords() { return _textureCoords; }
        LOADER_API float* getNormals() { return _normals; }
        LOADER_API uint getIndicesSize() { return _indicesSize; }
        LOADER_API uint* getIndices() { return _indices; }
        LOADER_API char* getMaterialName() { return _materialName; }
    };
}

#endif