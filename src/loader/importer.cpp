#include <phi\core\base64.h>
#include <phi\io\path.h>
#include <phi\rendering\model.h>
#include <phi\rendering\mesh.h>
#include <phi\loader\importer.h>
#include <phi\loader\SDL_extensions.h>

#include <rapidjson\filereadstream.h>
#include <iostream>
#include <fstream>
#include <objbase.h>
#include <SDL/SDL_image.h>

namespace phi
{
    int importer::importModel(std::string fileName, model*& resultModel)
    {
        auto modelName = path::getFileNameWithoutExtension(fileName);
        GLuint meshCount = 0;

        std::vector<geometryData*>* geometriesData = new std::vector<geometryData*>();
        if (!importer::importMesh(fileName, geometriesData))
        {
            log("Failed to load mesh");
            return 0;
        }

        resultModel = new model(modelName);

        auto name = path::getFileNameWithoutExtension(fileName);
        auto dir = path::getDirectoryFullName(fileName);
        auto thumbnailPath = dir + "\\" + name + ".th";
        resource<texture>* thumbnailRes;
        if (importTexture(thumbnailPath, thumbnailRes))
        {

        }

        auto i = 0;
        for each (geometryData* data in *geometriesData)
        {
            auto meshName = modelName + "_mesh_" + std::to_string(meshCount++);

            auto g = new geometry(data);

            auto m = new mesh(meshName, g, material::default);

            resultModel->addChild(m);
        }

        return 1;
    }

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

    int importer::importObject3D(std::string fileName, object3D*& rootNode)
    {
        FILE* fp;
        fopen_s(&fp, fileName.c_str(), "rb"); // non-Windows use "r"
        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        rapidjson::Document d;
        d.ParseStream(is);

        auto currentFolder = path::getDirectoryFullName(fileName);
        rootNode = readNode(d, currentFolder);

        fclose(fp);

        return 1;
    }

    int importer::importGeometryData(std::string fileName, geometryData*& data)
    {
        std::ifstream iFile;
        iFile.open(fileName.c_str(), std::ios::in | std::ios::binary);

        if (!iFile.is_open())
        {
            std::cout << "ERROR: File " << fileName << "coult not be loaded." << std::endl;
            return 0;
        }

        auto intSize = sizeof(int);
        auto floatSize = (unsigned int)sizeof(float);

        int verticesCount = -1;
        iFile.read(reinterpret_cast<char*>(&verticesCount), intSize);

        unsigned int positionsSize = verticesCount * 3 * floatSize;
        unsigned int texCoordsSize = verticesCount * 2 * floatSize;
        unsigned int normalsSize = verticesCount * 3 * floatSize;

        float* positionsBuffer = new float[verticesCount * 3];
        float* texCoordsBuffer = new float[verticesCount * 2];
        float* normalsBuffer = new float[verticesCount * 3];

        iFile.read(reinterpret_cast<char*>(positionsBuffer), positionsSize);
        iFile.read(reinterpret_cast<char*>(texCoordsBuffer), texCoordsSize);
        iFile.read(reinterpret_cast<char*>(normalsBuffer), normalsSize);

        int indicesCount = -1;
        iFile.read(reinterpret_cast<char*>(&indicesCount), intSize);

        unsigned int* indicesBuffer = new unsigned int[indicesCount];
        iFile.read(reinterpret_cast<char*>(indicesBuffer), indicesCount * intSize);

        data = geometryData::create(verticesCount, positionsBuffer, texCoordsBuffer, normalsBuffer, indicesCount, indicesBuffer);

        return 1;
    }

    int importer::importTexture(std::string fileName, texture*& tex)
    {
        SDL_Surface* surface = IMG_Load(fileName.c_str());
        SDL_InvertSurface(surface);

        GLenum format = 0;
        switch (surface->format->BitsPerPixel)
        {
            case 24:
                if (surface->format->Rmask == 255)
                    format = GL_RGB;
                else
                    format = GL_BGR;
                break;
            case 32:
                if (surface->format->Rmask == 255)
                    format = GL_RGBA;
                else
                    format = GL_BGRA;
                break;
        }

        auto totalBytes = surface->format->BitsPerPixel / 8;
        auto data = malloc(surface->w * surface->h * totalBytes);
        memcpy(data, surface->pixels, surface->w * surface->h * totalBytes);

        tex = new texture((uint)surface->w, (uint)surface->h, GL_RGBA, format, GL_UNSIGNED_BYTE, (byte*)data);

        SDL_FreeSurface(surface);

        return 1;
    }

    int importer::importTexture(std::string fileName, resource<texture>*& textureResource)
    {
        FILE* fp;
        fopen_s(&fp, fileName.c_str(), "rb"); // non-Windows use "r"
        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        rapidjson::Document d;
        d.ParseStream(is);

        auto guidBase64 = d["Guid"].GetString();
        auto guidBytes = base64::decode(guidBase64);
        auto guid = *reinterpret_cast<GUID*>(guidBytes.data());
        auto imageFileName = d["ImageFileName"].GetString();

        fclose(fp);

        auto name = path::getFileNameWithoutExtension(fileName);

        texture* tex;
        if (!importTexture(imageFileName, tex))
        {
            log("Image " + std::string(imageFileName) + " from texture " + name + " could not be loaded.");
            return 0;
        }

        textureResource = new resource<texture>(guid, name, tex);

        return 1;
    }

    int importer::importMaterial(std::string filename, resource<material>*& materialResource)
    {
        std::ifstream iFile(filename, std::ios::in | std::ios::binary);

        if (!iFile.is_open())
        {
            materialResource = nullptr;
            return 0;
        }

        iFile.seekg(0);

        GUID guid; // To create a new GUID use HRESULT hCreateGuid = CoCreateGuid(&gidReference);
        char* diffuseTextureName = new char[256];
        char* normalTextureName = new char[256];
        char* specularTextureName = new char[256];
        char* emissiveTextureName = new char[256];
        color ambientColor;
        color diffuseColor;
        color specularColor;
        color emissiveColor;
        float ka;
        float kd;
        float ks;
        float shininess;
        float reflectivity;
        bool isEmissive;

        iFile.read(reinterpret_cast<char*>(&guid), 16);
        iFile.read(diffuseTextureName, 256);
        iFile.read(normalTextureName, 256);
        iFile.read(specularTextureName, 256);
        iFile.read(emissiveTextureName, 256);
        iFile.read(reinterpret_cast<char*>(&ambientColor), sizeof(color));
        iFile.read(reinterpret_cast<char*>(&diffuseColor), sizeof(color));
        iFile.read(reinterpret_cast<char*>(&specularColor), sizeof(color));
        iFile.read(reinterpret_cast<char*>(&emissiveColor), sizeof(color));
        iFile.read(reinterpret_cast<char*>(&shininess), sizeof(float));
        iFile.read(reinterpret_cast<char*>(&reflectivity), sizeof(float));
        iFile.read(reinterpret_cast<char*>(&ka), sizeof(float));
        iFile.read(reinterpret_cast<char*>(&kd), sizeof(float));
        iFile.read(reinterpret_cast<char*>(&ks), sizeof(float));
        iFile.read(reinterpret_cast<char*>(&isEmissive), sizeof(bool));

        iFile.close();
        auto name = path::getFileNameWithoutExtension(filename);
        auto dir = path::getDirectoryFullName(filename);
        auto thumbnailPath = dir + "\\" + name + ".th";

        texture* thumbnailRes;
        if (importTexture(thumbnailPath, thumbnailRes))
        {

        }

        auto mat = new material(
            std::string(diffuseTextureName),
            std::string(normalTextureName),
            std::string(specularTextureName),
            std::string(emissiveTextureName),
            ambientColor,
            diffuseColor,
            specularColor,
            emissiveColor,
            ka,
            kd,
            ks,
            shininess,
            reflectivity,
            isEmissive);

        materialResource = new resource<material>(guid, name, mat);

        return 1;
    }

    object3D* importer::readNode(const rapidjson::Value& node, std::string currentFolder)
    {
        auto type = node["Type"].GetInt();
        object3D* objectNode = nullptr;

        switch (type)
        {
            case 0:
            {
                auto model = new phi::model(node["Name"].GetString());
                objectNode = model;
                break;
            }
            case 1:
            {
                geometryData* geometryData = nullptr;
                importGeometryData(currentFolder + "\\" + node["GeometryDataPath"].GetString(), geometryData);

                auto guidBase64 = node["MaterialGuid"].GetString();
                auto guidBytes = base64::decode(guidBase64);
                auto materialGuid = *reinterpret_cast<GUID*>(guidBytes.data());

                auto mesh = new phi::mesh(node["Name"].GetString(), new geometry(geometryData), nullptr);
                objectNode = mesh;
                break;
            }
        }

        if (!node.HasMember("Children"))
            return objectNode;

        const rapidjson::Value& children = node["Children"];
        for (rapidjson::SizeType i = 0; i < children.Size(); i++)
        {
            auto child = readNode(children[i], currentFolder);
            objectNode->addChild(child);
        }

        return objectNode;
    }
}