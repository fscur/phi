#include <precompiled.h>
#include "importer.h"

#include <core\base64.h>
#include <io\path.h>

#include "importResourceException.h"

namespace phi
{
    using rapidjson::FileReadStream;
    using rapidjson::Document;
    using rapidjson::Value;

    material* importer::defaultMaterial = nullptr;
    texture* importer::defaultAlbedoTexture = nullptr;
    texture* importer::defaultNormalTexture = nullptr;
    texture* importer::defaultSpecularTexture = nullptr;
    texture* importer::defaultEmissiveTexture = nullptr;

    node* importer::readNode(const rapidjson::Value& jsonNode, string currentFolder, resourcesRepository<material>* materialsRepo, resourcesRepository<geometry>* geometriesRepo)
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
                    auto geometryGuid = convertToGuid(components[i]["GeometryResourceGuid"].GetString());
                    auto geometry = geometriesRepo->getResource(geometryGuid)->getObject();

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
            auto child = readNode(children[i], currentFolder, materialsRepo, geometriesRepo);
            objectNode->addChild(child);
        }

        return objectNode;
    }

    guid importer::convertToGuid(const char* bytesGuid)
    {
        auto guidBytes = base64::decode(bytesGuid);
        return guidBytes.data();
    }

    resource<node>* importer::importNode(string fileName, resourcesRepository<material>* materialsRepo, resourcesRepository<geometry>* geometriesRepo)
    {
        //TODO:: create load file fuction in the io API
#ifdef _WIN32 
        FILE* file;
        fopen_s(&file, fileName.c_str(), "rb"); // non-Windows use "r"

        char fileBuffer[65536];
        FileReadStream fileStream(file, fileBuffer, sizeof(fileBuffer));
        Document document;
        document.ParseStream(fileStream);

        fclose(file);

        auto currentFolder = path::getDirectoryFullName(fileName);
        auto nodeName = path::getFileNameWithoutExtension(fileName);
        auto rootNode = readNode(document["Node"], currentFolder, materialsRepo, geometriesRepo);
        auto guid = convertToGuid(document["Guid"].GetString());

        return new resource<node>(guid, nodeName, rootNode);
#else
        throw importResourceException("importNode was not implemented in other platforms than WIN32", fileName);
#endif
    }

    texture* importer::importImage(string fileName)
    {
        auto fileNameChar = fileName.c_str();
        auto imageFormat = FreeImage_GetFileType(fileNameChar, 0);

        if (imageFormat == FIF_UNKNOWN)
            imageFormat = FreeImage_GetFIFFromFilename(fileNameChar);

        if (imageFormat == FIF_UNKNOWN)
            throw importResourceException("Format not identified importing texture", fileName);

        if (!FreeImage_FIFSupportsReading(imageFormat))
            throw importResourceException("Image format not supported ", fileName);
            
        auto imagePointer = FreeImage_Load(imageFormat, fileNameChar);
        if (!imagePointer)
            throw importResourceException("Failed loading image", fileName);

        auto dataPtr = FreeImage_GetBits(imagePointer);
        auto width = FreeImage_GetWidth(imagePointer);
        auto height = FreeImage_GetHeight(imagePointer);

        if (dataPtr == nullptr || width == 0 || height == 0)
            throw importResourceException("Failed loading image data of", fileName);

        auto format = GL_BGRA;
        auto bpp = FreeImage_GetBPP(imagePointer);
        auto redMask = FreeImage_GetRedMask(imagePointer);

        switch (bpp)
        {
        case 24:
            if (redMask == 255)
                format = GL_RGB;
            else
                format = GL_BGR;
            break;
        case 32:
            if (redMask == 255)
                format = GL_RGBA;
            else
                format = GL_BGRA;
            break;
        }

        auto totalBytes = bpp / 8;
        auto data = malloc(width * height * totalBytes);
        memcpy(data, dataPtr, width * height * totalBytes);

        return new texture(
            width,
            height,
            GL_TEXTURE_2D,
            GL_RGBA8,
            format,
            GL_UNSIGNED_BYTE,
            (byte*)dataPtr);
    }

    resource<texture>* importer::importTexture(string fileName)
    {
#ifdef _WIN32
        FILE* file;
        fopen_s(&file, fileName.c_str(), "rb"); // non-Windows use "r"
        char readBuffer[65536];

        FileReadStream fileStream(file, readBuffer, sizeof(readBuffer));
        Document document;
        document.ParseStream(fileStream);

        fclose(file);

        auto guid = convertToGuid(document["Guid"].GetString());
        auto imageName = document["ImageFileName"].GetString();

        auto imageFileName = path::getDirectoryFullName(fileName) + "\\" + imageName;
        auto textureName = path::getFileNameWithoutExtension(fileName);
        auto tex = importImage(imageFileName);

        return new resource<texture>(guid, textureName, tex);
#else
        throw importResourceException("Import texture was not implemented in other platforms than Win32", fileName);
#endif

    }

    resource<geometry>* importer::importGeometry(string fileName)
    {
#ifdef _WIN32
        std::ifstream file;
        file.open(fileName.c_str(), std::ios::in | std::ios::binary);

        if (!file.is_open())
        {
            throw importResourceException("File coult not be loaded.", fileName);
        }

        auto geomGuid = new uint8_t[16];
        file.read(reinterpret_cast<char*>(geomGuid), 16);

        int verticesCount = -1;
        file.read(reinterpret_cast<char*>(&verticesCount), sizeof(int));

        auto positionsBuffer = new float[verticesCount * 3];
        auto texCoordsBuffer = new float[verticesCount * 2];
        auto normalsBuffer = new float[verticesCount * 3];

        auto positionsSize = verticesCount * 3 * sizeof(float);
        auto texCoordsSize = verticesCount * 2 * sizeof(float);
        auto normalsSize = verticesCount * 3 * sizeof(float);

        file.read(reinterpret_cast<char*>(positionsBuffer), positionsSize);
        file.read(reinterpret_cast<char*>(texCoordsBuffer), texCoordsSize);
        file.read(reinterpret_cast<char*>(normalsBuffer), normalsSize);

        int indicesCount = -1;
        file.read(reinterpret_cast<char*>(&indicesCount), sizeof(int));

        auto indicesBuffer = new uint[indicesCount];
        file.read(reinterpret_cast<char*>(indicesBuffer), indicesCount * sizeof(int));

        file.close();

        auto name = path::getFileNameWithoutExtension(fileName);
        auto geom = geometry::create(verticesCount, positionsBuffer, texCoordsBuffer, normalsBuffer, indicesCount, indicesBuffer);
        return new resource<geometry>(geomGuid, name, geom);
#else
        throw importResourceException("importGeometry was not implemented ins other platforms than WIN32");
#endif
    }

    resource<material>* importer::importMaterial(string fileName, resourcesRepository<texture>* texturesRepo)
    {
#ifdef _WIN32
        FILE* file;
        fopen_s(&file, fileName.c_str(), "rb"); // non-Windows use "r"
        
        char readBuffer[65536];
        FileReadStream fileStream(file, readBuffer, sizeof(readBuffer));
        Document document;
        document.ParseStream(fileStream);

        fclose(file);

        auto guid = convertToGuid(document["Guid"].GetString());
        auto albedoTextureGuid = convertToGuid(document["AlbedoTextureGuid"].GetString());
        auto normalTextureGuid = convertToGuid(document["NormalTextureGuid"].GetString());
        auto specularTextureGuid = convertToGuid(document["SpecularTextureGuid"].GetString());
        auto emissiveTextureGuid = convertToGuid(document["EmissiveTextureGuid"].GetString());
        auto albedoTextureResource = texturesRepo->getResource(albedoTextureGuid);
        auto normalTextureResource = texturesRepo->getResource(normalTextureGuid);
        auto specularTextureResource = texturesRepo->getResource(specularTextureGuid);
        auto emissiveTextureResource = texturesRepo->getResource(emissiveTextureGuid);

        const Value& albedoColorNode = document["AlbedoColor"];
        const Value& specularColorNode = document["SpecularColor"];
        const Value& emissiveColorNode = document["EmissiveColor"];
        auto albedoColor = vec3((float)albedoColorNode[0].GetDouble(), (float)albedoColorNode[1].GetDouble(), (float)albedoColorNode[2].GetDouble());
        auto specularColor = vec3((float)specularColorNode[0].GetDouble(), (float)specularColorNode[1].GetDouble(), (float)specularColorNode[2].GetDouble());
        auto emissiveColor = vec3((float)emissiveColorNode[0].GetDouble(), (float)emissiveColorNode[1].GetDouble(), (float)emissiveColorNode[2].GetDouble());

        auto shininess = static_cast<float>(document["Shininess"].GetDouble());
        auto reflectivity = static_cast<float>(document["Reflectivity"].GetDouble());
        auto emission = static_cast<float>(document["Emission"].GetDouble());
        auto opacity = static_cast<float>(document["Opacity"].GetDouble());

        auto albedoTexture = albedoTextureResource == nullptr ? defaultAlbedoTexture : albedoTextureResource->getObject();
        auto normalTexture = normalTextureResource == nullptr ? defaultNormalTexture : normalTextureResource->getObject();
        auto specularTexture = specularTextureResource == nullptr ? defaultSpecularTexture : specularTextureResource->getObject();
        auto emissiveTexture = emissiveTextureResource == nullptr ? defaultEmissiveTexture : emissiveTextureResource->getObject();

        auto materialName = path::getFileNameWithoutExtension(fileName);
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

        return new resource<material>(guid, materialName, mat);
#else
        throw importResourceException("Import material was not implemented in other platforms than Win32", fileName);
#endif
    }
}