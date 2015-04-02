#include "model.h"
#include "globals.h"
#include "path.h"
#include <fstream>
#include <iostream>

namespace phi
{
    model::model(std::string name, std::string path) :
        resource(name, path)
    {
    }

    void model::addMesh(mesh* mesh)
    {
        _meshes.push_back(mesh);
    }

    model* model::fromFile(std::string fileName)
    {
        /*LOG("fromFile: " << fileName);

		if (!globals::contains(fileName, ".model"))
		{
			LOG("fromFile: Invalid file format.");
			return nullptr;
		}*/

        auto materialName = new char[256];

	    std::ifstream iFile;
	    iFile.open(fileName.c_str(), std::ios::in | std::ios::binary);

        if (!iFile.is_open())
        {
            LOG("fromFile: Could not load the file.");
            return nullptr;
        }

        iFile.seekg(0);

        auto uintSize = sizeof(GLuint);
        auto floatSize = sizeof(GLfloat);
        auto modelName = path::getFileNameWithoutExtension(fileName);
        GLuint meshCount = 0;

        model* md = new model(modelName, fileName);
        
        while(iFile.peek() != -1)
        {
            auto meshName = modelName + "_mesh_" + std::to_string(meshCount++);
            
            iFile.read(materialName, 256);

            GLuint verticesCount = -1;
            iFile.read(reinterpret_cast<char*>(&verticesCount), uintSize);
            
            GLuint positionsSize = verticesCount * 3 * floatSize;
		    GLuint texCoordsSize = verticesCount * 2 * floatSize;
		    GLuint normalsSize = verticesCount * 3 * floatSize;

            GLfloat* positionsBuffer = new GLfloat[verticesCount * 3];
		    GLfloat* texCoordsBuffer = new GLfloat[verticesCount * 2];
		    GLfloat* normalsBuffer = new GLfloat[verticesCount * 3];

            iFile.read(reinterpret_cast<char*>(positionsBuffer), positionsSize);
            iFile.read(reinterpret_cast<char*>(texCoordsBuffer), texCoordsSize);
            iFile.read(reinterpret_cast<char*>(normalsBuffer), normalsSize);

            GLuint indicesCount = -1;
            iFile.read(reinterpret_cast<char*>(&indicesCount), uintSize);

            GLuint* indicesBuffer = new GLuint[indicesCount];
            iFile.read(reinterpret_cast<char*>(indicesBuffer), indicesCount * sizeof(unsigned int));

            mesh* m = mesh::create(verticesCount, positionsBuffer, texCoordsBuffer, normalsBuffer, indicesCount, indicesBuffer, materialName);
            
            md->_meshes.push_back(m);
        }

        return md;
    }
}