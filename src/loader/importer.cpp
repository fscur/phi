#include "importer.h"

#include <core\base64.h>
#include <io\path.h>
#include <loader\SDL_extensions.h>

#include <rapidjson\filereadstream.h>
#include <iostream>
#include <fstream>
#include <objbase.h>
#include <SDL/SDL_image.h>

namespace phi
{
    object3D* importer::readNode(const rapidjson::Value& node, std::string currentFolder, resourcesRepository<material>* materialsRepo)
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
                auto geometryPath = node["GeometryPath"].GetString();
                phi::geometry* geometry = nullptr;
                importGeometry(currentFolder + "\\" + geometryPath, geometry);

                auto materialGuid = convertToGuid(node["MaterialGuid"].GetString());
                auto matRes = materialsRepo->getResource(materialGuid);

                material* mat;
                if (matRes == nullptr)
                    mat = material::default();
                else
                    mat = matRes->getObject();

                auto mesh = new phi::mesh(node["Name"].GetString(), geometry, mat);
                objectNode = mesh;
                break;
            }
        }

        if (!node.HasMember("Children"))
            return objectNode;

        const rapidjson::Value& children = node["Children"];
        auto childrenCount = children.Size();
        for (rapidjson::SizeType i = 0; i < childrenCount; i++)
        {
            auto child = readNode(children[i], currentFolder, materialsRepo);
            objectNode->addChild(child);
        }

        return objectNode;
    }

    GUID importer::convertToGuid(const char* bytesGuid)
    {
        auto guidBytes = base64::decode(bytesGuid);
        return *reinterpret_cast<GUID*>(guidBytes.data());
    }

    int importer::importModel(std::string fileName, model*& resultModel)
    {
        auto modelName = path::getFileNameWithoutExtension(fileName);
        GLuint meshCount = 0;

        std::vector<geometry*>* geometries = new std::vector<geometry*>();
        if (!importer::importMesh(fileName, geometries))
        {
            log("Failed to load mesh:" + modelName);
            return 0;
        }

        resultModel = new model(modelName);

        auto name = path::getFileNameWithoutExtension(fileName);
        auto dir = path::getDirectoryFullName(fileName);
        auto thumbnailPath = dir + "\\" + name + ".th";
        resource<texture>* thumbnailRes;
        if (importTexture(thumbnailPath, thumbnailRes))
        {
            //TODO: badass if!
        }

        auto i = 0;
        for (auto geometry : *geometries)
        {
            auto meshName = modelName + "_mesh_" + std::to_string(meshCount++);

            auto m = new mesh(meshName, geometry, material::default());

            resultModel->addChild(m);
        }

        return 1;
    }

    int importer::importMesh(std::string fileName, std::vector<geometry*>* data)
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
        *data = std::vector<geometry*>();

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

            auto geometry = geometry::create(verticesCount, positionsBuffer, texCoordsBuffer, normalsBuffer, indicesCount, indicesBuffer);

            data->push_back(geometry);
        }

        return 1;
    }

    int importer::importObject3D(std::string fileName, resource<object3D>*& objectResource, resourcesRepository<material>* materialsRepo)
    {
        FILE* fp;
        fopen_s(&fp, fileName.c_str(), "rb"); // non-Windows use "r"
        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        rapidjson::Document d;
        d.ParseStream(is);

        auto currentFolder = path::getDirectoryFullName(fileName);
        auto rootNode = readNode(d, currentFolder, materialsRepo);

        auto guid = convertToGuid(d["Guid"].GetString());

        objectResource = new resource<object3D>(guid, path::getFileNameWithoutExtension(fileName), rootNode);

        fclose(fp);

        return 1;
    }

    int importer::importGeometry(std::string fileName, geometry*& data)
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

        data = geometry::create(verticesCount, positionsBuffer, texCoordsBuffer, normalsBuffer, indicesCount, indicesBuffer);

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

        auto guid = convertToGuid(d["Guid"].GetString());
        auto imageFileName = path::getDirectoryFullName(fileName) + "\\" + d["ImageFileName"].GetString();

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

    int importer::importMaterial(std::string fileName, resource<material>*& materialResource, resourcesRepository<texture>* texturesRepo)
    {
        FILE* fp;
        fopen_s(&fp, fileName.c_str(), "rb"); // non-Windows use "r"
        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        rapidjson::Document d;
        d.ParseStream(is);

        auto guid = convertToGuid(d["Guid"].GetString());
        auto albedoTextureGuid = convertToGuid(d["AlbedoTextureGuid"].GetString());
        auto normalTextureGuid = convertToGuid(d["NormalTextureGuid"].GetString());
        auto specularTextureGuid = convertToGuid(d["SpecularTextureGuid"].GetString());
        auto emissiveTextureGuid = convertToGuid(d["EmissiveTextureGuid"].GetString());
        auto albedoTexture = texturesRepo->getResource(albedoTextureGuid);
        auto normalTexture = texturesRepo->getResource(normalTextureGuid);
        auto specularTexture = texturesRepo->getResource(specularTextureGuid);
        auto emissiveTexture = texturesRepo->getResource(emissiveTextureGuid);

        const rapidjson::Value& albedoColorNode = d["AlbedoColor"];
        const rapidjson::Value& specularColorNode = d["SpecularColor"];
        const rapidjson::Value& emissiveColorNode = d["EmissiveColor"];
        auto albedoColor = vec3((float)albedoColorNode["R"].GetDouble(), (float)albedoColorNode["G"].GetDouble(), (float)albedoColorNode["B"].GetDouble());
        auto specularColor = vec3((float)specularColorNode["R"].GetDouble(), (float)specularColorNode["G"].GetDouble(), (float)specularColorNode["B"].GetDouble());
        auto emissiveColor = vec3((float)emissiveColorNode["R"].GetDouble(), (float)emissiveColorNode["G"].GetDouble(), (float)emissiveColorNode["B"].GetDouble());

        auto shininess = (float)d["Shininess"].GetDouble();
        auto reflectivity = (float)d["Reflectivity"].GetDouble();
        auto emission = d["Emission"].GetDouble();
        auto opacity = (float)d["Opacity"].GetDouble();

        fclose(fp);

        auto mat = new material(
            albedoTexture == nullptr ? texture::defaultAlbedo() : albedoTexture->getObject(),
            normalTexture == nullptr ? texture::defaultNormal() : normalTexture->getObject(),
            specularTexture == nullptr ? texture::defaultSpecular() : specularTexture->getObject(),
            emissiveTexture == nullptr ? texture::defaultEmissive() : emissiveTexture->getObject(),
            albedoColor,
            specularColor,
            emissiveColor,
            shininess,
            reflectivity,
            emission,
            opacity);

        materialResource = new resource<material>(guid, path::getFileNameWithoutExtension(fileName), mat);

        return 1;
    }
}