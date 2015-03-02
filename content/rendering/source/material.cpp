#include "material.h"
#include "renderingSystem.h"
#include <fstream>

namespace phi
{
    material::material(
        std::string name,
        std::string path,
        std::string diffuseTextureName,
		std::string normalTextureName,
        std::string specularTextureName, 
        color ambientColor,
        color diffuseColor, 
        color specularColor, 
        float ka,
        float kd,
        float ks,
        float shininess) :
        resource(name, path)
    {
        _diffuseTextureName = diffuseTextureName;
        _normalTextureName = normalTextureName;
        _specularTextureName = specularTextureName;
        _ambientColor = ambientColor;
        _diffuseColor = diffuseColor;
        _specularColor = specularColor; 
        _ka = ka;
        _kd = kd;
        _ks = ks;
        _shininess = shininess;
    }

    material::~material()
    {}

    void material::save(std::string filename)
    {
        std::ofstream oFile(filename, std::ios::out | std::ios::binary);

        if (!oFile.is_open())
            return;
    
        oFile.clear();
        oFile.seekp(0);

        oFile.write(_name.c_str(), 256);
        oFile.write(_diffuseTextureName.c_str(), 256);
        oFile.write(_normalTextureName.c_str(), 256);
        oFile.write(_specularTextureName.c_str(), 256);
        oFile.write(reinterpret_cast<char*>(&_ambientColor), sizeof(color));
        oFile.write(reinterpret_cast<char*>(&_diffuseColor), sizeof(color));
        oFile.write(reinterpret_cast<char*>(&_specularColor), sizeof(color));
        oFile.write(reinterpret_cast<char*>(&_ka), sizeof(float));
        oFile.write(reinterpret_cast<char*>(&_kd), sizeof(float));
        oFile.write(reinterpret_cast<char*>(&_ks), sizeof(float));
        oFile.write(reinterpret_cast<char*>(&_shininess), sizeof(float));

        oFile.close();
    }

    material* material::fromFile(std::string filename)
    {
        std::ifstream iFile(filename, std::ios::in | std::ios::binary);

        if (!iFile.is_open())
            return nullptr;
    
        iFile.seekg(0);

        char* name = new char[256];
        char* diffuseTextureName = new char[256];
        char* normalTextureName = new char[256];
        char* specularTextureName = new char[256];
        color ambientColor;
        color diffuseColor;
        color specularColor;
        float ka;
        float kd;
        float ks;
        float shininess;

        iFile.read(name, 256);
        iFile.read(diffuseTextureName, 256);
        iFile.read(normalTextureName, 256);
        iFile.read(specularTextureName, 256);
        iFile.read(reinterpret_cast<char*>(&ambientColor), sizeof(color));
        iFile.read(reinterpret_cast<char*>(&diffuseColor), sizeof(color));
        iFile.read(reinterpret_cast<char*>(&specularColor), sizeof(color));
        iFile.read(reinterpret_cast<char*>(&ka), sizeof(float));
        iFile.read(reinterpret_cast<char*>(&kd), sizeof(float));
        iFile.read(reinterpret_cast<char*>(&ks), sizeof(float));
        iFile.read(reinterpret_cast<char*>(&shininess), sizeof(float));
    
        iFile.close();

        return new material(
            std::string(name),
            std::string(filename),
            std::string(diffuseTextureName), 
            std::string(normalTextureName), 
            std::string(specularTextureName), 
            ambientColor, 
            diffuseColor, 
            specularColor, 
            ka,
            kd,
            ks,
            shininess); 
    }
}