#ifndef _PHI_MATERIAL_H_
#define _PHI_MATERIAL_H_

#include <phi/core/color.h>

#include "texture.h"

namespace phi
{
    class material :
        public resource
    {
    private:
        GLuint64 _diffuseHandle;
        std::string _diffuseTextureName;
        std::string _normalTextureName;
        std::string _specularTextureName;
        std::string _emissiveTextureName;
        texture* _diffuseTexture;
        texture* _normalTexture;
        texture* _specularTexture;
        texture* _emissiveTexture;
        texture* _thumbnail;
        color _ambientColor;
        color _diffuseColor;
        color _specularColor;
        color _emissiveColor;
        float _ka;
        float _kd;
        float _ks;
        float _shininess;
        float _reflectivity;
        bool _isEmissive;

    private:
        static material* defaultMaterial;

    public:
        RENDERING_API static material* getDefault();
        RENDERING_API static material* getLambert(color color);

    public:
        RENDERING_API material(
            std::string name,
            std::string path,
            texture* diffuseTexture,
            texture* normalTexture,
            texture* specularTexture,
            texture* emissiveTexture,
            color ambientColor = color::white,
            color diffuseColor = color::white,
            color specularColor = color::white,
            color emissiveColor = color::white,
            float ka = 0.0f,
            float kd = 0.0f,
            float ks = 0.0f,
            float shininess = 0.0f,
            float reflectivity = 0.0f,
            bool isEmissive = false,
            texture* thumbnail = nullptr);

        RENDERING_API material(
            std::string name,
            std::string path,
            std::string diffuseTextureName,
            std::string normalTextureName,
            std::string specularTextureName,
            std::string emissiveTextureName,
            color ambientColor = color::white,
            color diffuseColor = color::white,
            color specularColor = color::white,
            color emissiveColor = color::white,
            float ka = 0.0f,
            float kd = 0.0f,
            float ks = 0.0f,
            float shininess = 0.0f,
            float reflectivity = 0.0f,
            bool isEmissive = false,
            texture* thumbnail = nullptr);

        RENDERING_API ~material();
        RENDERING_API std::string getDiffuseTextureName() const { return _diffuseTextureName; }
        RENDERING_API std::string getNormalTextureName() const { return _normalTextureName; }
        RENDERING_API std::string getSpecularTextureName() const { return _specularTextureName; }
        RENDERING_API std::string getEmissiveTextureName() const { return _emissiveTextureName; }
        RENDERING_API texture* getDiffuseTexture() const { return _diffuseTexture; }
        RENDERING_API texture* getNormalTexture() const { return _normalTexture; }
        RENDERING_API texture* getSpecularTexture() const { return _specularTexture; }
        RENDERING_API texture* getEmissiveTexture() const { return _emissiveTexture; }
        RENDERING_API color getAmbientColor() const { return _ambientColor; }
        RENDERING_API color getDiffuseColor() const { return _diffuseColor; }
        RENDERING_API color getSpecularColor() const { return _specularColor; }
        RENDERING_API color getEmissiveColor() const { return _emissiveColor; }
        RENDERING_API float getKa() const { return _ka; }
        RENDERING_API float getKd() const { return _kd; }
        RENDERING_API float getKs() const { return _ks; }
        RENDERING_API float getShininess() const { return _shininess; }
        RENDERING_API float getReflectivity() const { return _reflectivity; }
        RENDERING_API bool getIsEmissive() const { return _isEmissive; }
        RENDERING_API texture* getThumbnail() const { return _thumbnail; }
        RENDERING_API GLuint64 getDiffuseHandle() const { return _diffuseHandle; }


        RENDERING_API void setDiffuseHandle(GLuint64 value) { _diffuseHandle = value; }
        RENDERING_API void setDiffuseTextureName(std::string value) { _diffuseTextureName = value; }
        RENDERING_API void setNormalTextureName(std::string value) { _normalTextureName = value; }
        RENDERING_API void setSpecularTextureName(std::string value) { _specularTextureName = value; }
        RENDERING_API void setEmissiveTextureName(std::string value) { _emissiveTextureName = value; }
        RENDERING_API void setDiffuseTexture(texture* value) { _diffuseTexture = value; }
        RENDERING_API void setNormalTexture(texture* value) { _normalTexture = value; }
        RENDERING_API void setSpecularTexture(texture* value) { _specularTexture = value; }
        RENDERING_API void setEmissiveTexture(texture* value) { _emissiveTexture = value; }
        RENDERING_API void setAmbientColor(color value) { _ambientColor = value; }
        RENDERING_API void setDiffuseColor(color value) { _diffuseColor = value; }
        RENDERING_API void setSpecularColor(color value) { _specularColor = value; }
        RENDERING_API void setEmissiveColor(color value) { _emissiveColor = value; }
        RENDERING_API void setKa(float value) { _ka = value; }
        RENDERING_API void setKd(float value) { _kd = value; }
        RENDERING_API void setKs(float value) { _ks = value; }
        RENDERING_API void setShininess(float value) { _shininess = value; }
        RENDERING_API void setReflectivity(float value) { _reflectivity = value; }
        RENDERING_API void setIsEmissive(bool value) { _isEmissive = value; }
        RENDERING_API void setThumbnail(texture* value) { _thumbnail = value; }

        RENDERING_API void save(std::string filename);

        RENDERING_API static material* fromFile(std::string filename);
    };
}
#endif