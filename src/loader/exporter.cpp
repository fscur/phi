#include <phi/loader/exporter.h>

#include <fstream>

namespace phi
{
    int exporter::exportMesh(std::vector<geometry*>* geometry, char* fileName)
    {
        char* mat = "Material is not used anymore.";
        std::ofstream stream;
        stream.open(fileName, std::ios::out | std::ios::binary);

        auto meshCount = (*geometry).size();
        for (unsigned int i = 0; i < meshCount; i++)
        {
            auto itemData = (*geometry)[i];
            stream.write(mat, 256);

            auto verticesCount = itemData->verticesCount;
            stream.write((char*)&verticesCount, sizeof(unsigned int));

            stream.write((char*)itemData->vboData, itemData->vboSize);

            auto indicesCount = itemData->indicesCount;
            stream.write((char*)&indicesCount, sizeof(unsigned int));
            stream.write((char*)itemData->eboData, itemData->eboSize);
        }

        delete[] mat;

        stream.close();

        return 1;
    }
}