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
    material* importer::defaultMaterial = nullptr;
    texture* importer::defaultAlbedoTexture = nullptr;
    texture* importer::defaultNormalTexture = nullptr;
    texture* importer::defaultSpecularTexture = nullptr;
    texture* importer::defaultEmissiveTexture = nullptr;

    node* importer::readNode(const rapidjson::Value& jsonNode, std::string currentFolder, resourcesRepository<material>* materialsRepo)
    {
        auto objectNode = new node();

        const rapidjson::Value& components = jsonNode["Components"];
        auto componentsCount = components.Size();
        for (rapidjson::SizeType i = 0; i < componentsCount; i++)
        {
            auto type = components[i]["Type"].GetInt();
            component* component = nullptr;

            switch (type)
            {
                case 0:
                {
                    component = new phi::model(components[i]["Name"].GetString());
                    break;
                }
                case 1:
                {
                    auto geometryPath = components[i]["GeometryPath"].GetString();
                    phi::geometry* geometry = nullptr;
                    importGeometry(currentFolder + "\\" + geometryPath, geometry);

                    auto materialGuid = convertToGuid(components[i]["MaterialResourceGuid"].GetString());
                    auto matRes = materialsRepo->getResource(materialGuid);

                    material* mat;
                    if (matRes == nullptr)
                        mat = defaultMaterial;
                    else
                        mat = matRes->getObject();

                    component = new phi::mesh(components[i]["Name"].GetString(), geometry, mat);
                    break;
                }
            }
            objectNode->addComponent(component);
        }

        const rapidjson::Value& children = jsonNode["Children"];
        auto childrenCount = children.Size();
        for (rapidjson::SizeType i = 0; i < childrenCount; i++)
        {
            auto child = readNode(children[i], currentFolder, materialsRepo);
            objectNode->addChild(child);
        }

        //auto type = node["Type"].GetInt();
        //object3D* objectNode = nullptr;

        //switch (type)
        //{
        //    case 0:
        //    {
        //        auto model = new phi::model(node["Name"].GetString());
        //        objectNode = model;
        //        break;
        //    }
        //    case 1:
        //    {
        //        auto geometryPath = node["GeometryPath"].GetString();
        //        phi::geometry* geometry = nullptr;
        //        importGeometry(currentFolder + "\\" + geometryPath, geometry);

        //        auto materialGuid = convertToGuid(node["MaterialGuid"].GetString());
        //        auto matRes = materialsRepo->getResource(materialGuid);

        //        material* mat;
        //        if (matRes == nullptr)
        //            mat = defaultMaterial;
        //        else
        //            mat = matRes->getObject();

        //        auto mesh = new phi::mesh(node["Name"].GetString(), geometry, mat);
        //        objectNode = mesh;
        //        break;
        //    }
        //}

        //if (!node.HasMember("Children"))
        //    return objectNode;

        //const rapidjson::Value& children = node["Children"];
        //auto childrenCount = children.Size();
        //for (rapidjson::SizeType i = 0; i < childrenCount; i++)
        //{
        //    auto child = readNode(children[i], currentFolder, materialsRepo);
        //    objectNode->addChild(child);
        //}

        return objectNode;
    }

    GUID importer::convertToGuid(const char* bytesGuid)
    {
        auto guidBytes = base64::decode(bytesGuid);
        return *reinterpret_cast<GUID*>(guidBytes.data());
    }

    int importer::importNode(std::string fileName, resource<node>*& objectResource, resourcesRepository<material>* materialsRepo)
    {
        FILE* fp;
        fopen_s(&fp, fileName.c_str(), "rb"); // non-Windows use "r"
        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        rapidjson::Document doc;
        doc.ParseStream(is);

        auto currentFolder = path::getDirectoryFullName(fileName);

        auto rootNode = readNode(doc["Node"], currentFolder, materialsRepo);
        auto guid = convertToGuid(doc["Guid"].GetString());

        objectResource = new resource<node>(guid, path::getFileNameWithoutExtension(fileName), rootNode);

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

    int importer::importTexture(std::string fileName, texture*& texture)
    {
        SDL_Surface* surface = IMG_Load(fileName.c_str());
        SDL_InvertSurface(surface);

        GLenum format = GL_BGRA;

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

        texture = new phi::texture(
            (uint)surface->w, 
            (uint)surface->h, 
            GL_TEXTURE_2D,
            GL_RGBA8,
            format, 
            GL_UNSIGNED_BYTE,
            (byte*)data);

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
        auto albedoTextureResource = texturesRepo->getResource(albedoTextureGuid);
        auto normalTextureResource = texturesRepo->getResource(normalTextureGuid);
        auto specularTextureResource = texturesRepo->getResource(specularTextureGuid);
        auto emissiveTextureResource = texturesRepo->getResource(emissiveTextureGuid);

        const rapidjson::Value& albedoColorNode = d["AlbedoColor"];
        const rapidjson::Value& specularColorNode = d["SpecularColor"];
        const rapidjson::Value& emissiveColorNode = d["EmissiveColor"];
        auto albedoColor = vec3((float)albedoColorNode[0].GetDouble(), (float)albedoColorNode[1].GetDouble(), (float)albedoColorNode[2].GetDouble());
        auto specularColor = vec3((float)specularColorNode[0].GetDouble(), (float)specularColorNode[1].GetDouble(), (float)specularColorNode[2].GetDouble());
        auto emissiveColor = vec3((float)emissiveColorNode[0].GetDouble(), (float)emissiveColorNode[1].GetDouble(), (float)emissiveColorNode[2].GetDouble());

        auto shininess = (float)d["Shininess"].GetDouble();
        auto reflectivity = (float)d["Reflectivity"].GetDouble();
        auto emission = d["Emission"].GetDouble();
        auto opacity = (float)d["Opacity"].GetDouble();

        fclose(fp);

        auto albedoTexture = albedoTextureResource == nullptr ? defaultAlbedoTexture : albedoTextureResource->getObject();
        auto normalTexture = normalTextureResource == nullptr ? defaultNormalTexture : normalTextureResource->getObject();
        auto specularTexture = specularTextureResource == nullptr ? defaultSpecularTexture : specularTextureResource->getObject();
        auto emissiveTexture = emissiveTextureResource == nullptr ? defaultEmissiveTexture : emissiveTextureResource->getObject();

        auto mat = new material(
            albedoTexture,
            normalTexture,
            specularTexture,
            emissiveTexture,
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