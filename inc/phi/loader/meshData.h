#ifndef _PHI_MESH_DATA_H_
#define _PHI_MESH_DATA_H_

#include "loader.h"

namespace phi
{
    class meshData
    {
    private:
        unsigned int _verticesSize;
        float* _positions;
        float* _textureCoords;
        float* _normals;
        unsigned int _indicesSize;
        unsigned int* _indices;
        char* _materialName;

    public:
        meshData(unsigned int verticesSize, float* positions, float* textureCoords, float* normals, unsigned int indicesSize, unsigned int* indices, char* materialName)
        {
            _verticesSize = verticesSize;
            _positions = positions;
            _textureCoords = textureCoords;
            _normals = normals;
            _indicesSize = indicesSize;
            _indices = indices;
            _materialName = materialName;
        }

        LOADER_API unsigned int getVerticesSize() { return _verticesSize; }
        LOADER_API float* getPositions() { return _positions; }
        LOADER_API float* getTextureCoords() { return _textureCoords; }
        LOADER_API float* getNormals() { return _normals; }
        LOADER_API unsigned int getIndicesSize() { return _indicesSize; }
        LOADER_API unsigned int* getIndices() { return _indices; }
        LOADER_API char* getMaterialName() { return _materialName; }
    };
}

#endif