#include <precompiled.h>
#include "importer.h"

#include <core\base64.h>
#include <io\path.h>

namespace phi
{
    material* importer::defaultMaterial = nullptr;
    texture* importer::defaultAlbedoTexture = nullptr;
    texture* importer::defaultNormalTexture = nullptr;
    texture* importer::defaultSpecularTexture = nullptr;
    texture* importer::defaultEmissiveTexture = nullptr;

    node* importer::readNode(const rapidjson::Value& jsonNode, string currentFolder, resourcesRepository<material>* materialsRepo)
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

    int importer::importNode(string fileName, resource<node>*& objectResource, resourcesRepository<material>* materialsRepo)
    {
        //TODO:: create load file fuction in the io API
#ifdef _WIN32 
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
#endif
        return 0;
    }

    int importer::importGeometry(string fileName, geometry*& data)
    {
#ifdef _WIN32
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
#endif
        return 0;
    }

    int importer::importTexture(string fileName, texture*& texture)
    {
        auto cfileName = fileName.c_str();
            //image format
            FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
            //pointer to the image, once loaded
            FIBITMAP *dib(0);
            //pointer to the image data
            BYTE* bits(0);
            //image width and height
            unsigned int width(0), height(0);
            //OpenGL's image ID to map to
            GLuint gl_texID;

            //check the file signature and deduce its format
            fif = FreeImage_GetFileType(cfileName, 0);
            //if still unknown, try to guess the file format from the file extension
            if (fif == FIF_UNKNOWN)
                fif = FreeImage_GetFIFFromFilename(cfileName);
            //if still unkown, return failure
            if (fif == FIF_UNKNOWN)
                return false;

            //check that the plugin has reading capabilities and load the file
            if (FreeImage_FIFSupportsReading(fif))
                dib = FreeImage_Load(fif, cfileName);
            //if the image failed to load, return failure
            if (!dib)
                return false;

            //retrieve the image data
            bits = FreeImage_GetBits(dib);
            //get the image width and height
            width = FreeImage_GetWidth(dib);
            height = FreeImage_GetHeight(dib);
            //if this somehow one of these failed (they shouldn't), return failure
            if ((bits == 0) || (width == 0) || (height == 0))
                return 0;

            GLenum format = GL_BGRA;
            auto bpp = FreeImage_GetBPP(dib);
            auto redMask = FreeImage_GetRedMask(dib);

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
            memcpy(data, bits, width * height * totalBytes);

            texture = new phi::texture(
                width,
                height,
                GL_TEXTURE_2D,
                GL_RGBA8,
                format,
                GL_UNSIGNED_BYTE,
                (byte*)data);

            return 1;
    }

    int importer::importTexture(string fileName, resource<texture>*& textureResource)
    {
#ifdef _WIN32
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
            log("Image " + string(imageFileName) + " from texture " + name + " could not be loaded.");
            return 0;
        }

        textureResource = new resource<texture>(guid, name, tex);
        return 1;
#endif
        return 0;
    }

    int importer::importMaterial(string fileName, resource<material>*& materialResource, resourcesRepository<texture>* texturesRepo)
    {
#ifdef _WIN32
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
#endif
        return 0;
    }
}