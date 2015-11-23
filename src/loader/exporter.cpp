#include "phi/loader/exporter.h"
#include <fstream>

namespace phi
{
    int exporter::exportMesh(std::vector<meshData*>* data, char* fileName)
    {
        std::ofstream stream;
        stream.open(fileName, std::ios::out | std::ios::binary);

        auto meshCount = (*data).size();
        for (unsigned int i = 0; i < meshCount; i++)
        {
            meshData* itemData = (*data)[i];
            stream.write(itemData->getMaterialName(), 256);

            auto verticesSize = itemData->getVerticesSize();
            stream.write((char*)&verticesSize, sizeof(unsigned int));

            stream.write((char*)itemData->getPositions(), verticesSize * 3 * sizeof(float));
            stream.write((char*)itemData->getTextureCoords(), verticesSize * 2 * sizeof(float));
            stream.write((char*)itemData->getNormals(), verticesSize * 3 * sizeof(float));

            auto indicesSize = itemData->getIndicesSize();
            stream.write((char*)&indicesSize, sizeof(unsigned int));
            stream.write((char*)itemData->getIndices(), indicesSize * sizeof(unsigned int));
        }

        stream.close();

        return 1;
    }
}