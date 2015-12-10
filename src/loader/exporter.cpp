#include <phi/loader/exporter.h>

#include <fstream>

namespace phi
{
    int exporter::exportMesh(std::vector<geometryData*>* data, char* fileName)
    {
        char* mat = "Material is not used anymore.";
        std::ofstream stream;
        stream.open(fileName, std::ios::out | std::ios::binary);

        auto meshCount = (*data).size();
        for (unsigned int i = 0; i < meshCount; i++)
        {
            auto itemData = (*data)[i];
            stream.write(mat, 256);

            auto verticesCount = itemData->getVerticesCount();
            stream.write((char*)&verticesCount, sizeof(unsigned int));

            stream.write((char*)itemData->getPositionsBuffer(), itemData->getPSize());
            stream.write((char*)itemData->getTexCoordsBuffer(), itemData->getTSize());
            stream.write((char*)itemData->getNormalsBuffer(), itemData->getNSize());

            auto indicesCount = itemData->getIndicesCount();
            stream.write((char*)&indicesCount, sizeof(unsigned int));
            stream.write((char*)itemData->getIndices(), itemData->getISize());
        }

        delete[] mat;

        stream.close();

        return 1;
    }
}