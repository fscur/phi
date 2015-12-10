#include <phi/loader/importer.h>

#include <iostream>
#include <fstream>

namespace phi
{
    int importer::importMesh(std::string fileName, std::vector<geometryData*>* data)
    {
        std::ifstream iFile;
        iFile.open(fileName.c_str(), std::ios::in | std::ios::binary);

        if (!iFile.is_open())
        {
            std::cout << "ERROR: File " << fileName << "coult not be loaded." << std::endl;
            return 0;
        }

        auto uintSize = sizeof(unsigned int);
        auto floatSize = (unsigned int)sizeof(float);
        *data = std::vector<geometryData*>();

        while (iFile.peek() != -1)
        {
            char* materialName = new char[256];
            iFile.read(materialName, 256);

            unsigned int verticesCount = -1;
            iFile.read(reinterpret_cast<char*>(&verticesCount), uintSize);

            unsigned int positionsSize = verticesCount * 3 * floatSize;
            unsigned int texCoordsSize = verticesCount * 2 * floatSize;
            unsigned int normalsSize = verticesCount * 3 * floatSize;

            float* positionsBuffer = new float[verticesCount * 3];
            float* texCoordsBuffer = new float[verticesCount * 2];
            float* normalsBuffer = new float[verticesCount * 3];

            iFile.read(reinterpret_cast<char*>(positionsBuffer), positionsSize);
            iFile.read(reinterpret_cast<char*>(texCoordsBuffer), texCoordsSize);
            iFile.read(reinterpret_cast<char*>(normalsBuffer), normalsSize);

            unsigned int indicesCount = -1;
            iFile.read(reinterpret_cast<char*>(&indicesCount), uintSize);

            unsigned int* indicesBuffer = new unsigned int[indicesCount];
            iFile.read(reinterpret_cast<char*>(indicesBuffer), indicesCount * uintSize);

            auto geometry = geometryData::create(verticesCount, positionsBuffer, texCoordsBuffer, normalsBuffer, indicesCount, indicesBuffer);

            data->push_back(geometry);
        }

        return 1;
    }
}