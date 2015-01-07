#ifndef _PHI_MATERIAL_H_
#define _PHI_MATERIAL_H_

#include "texture.h"
#include "color.h"

namespace phi
{
    class material :
        public resource
    {
    private:
        texture* _diffuseTexture;
		texture* _normalTexture;
        texture* _specularTexture;
        color _ambientColor;
        color _diffuseColor;
        color _specularColor;
        float _ka;
        float _kd;
        float _ks;
        float _shininess;

    public:
        RENDERING_API material(
            std::string name,
            std::string path,
            texture* diffuseTexture = nullptr, 
            texture* normalTexture = nullptr, 
            texture* specularTexture = nullptr, 
            color ambientColor = color::white,
            color diffuseColor = color::white, 
            color specularColor = color::white, 
            float ka = 0,
            float kd = 0,
            float ks = 0,
            float shininess = 0);
        RENDERING_API ~material();

        RENDERING_API texture* getDiffuseTexture() const { return _diffuseTexture; }
		RENDERING_API texture* getNormalTexture() const { return _normalTexture; }

        RENDERING_API texture* getSpecularTexture() const { return _specularTexture; }
        RENDERING_API color getAmbientColor() const { return _ambientColor; }
        RENDERING_API color getDiffuseColor() const { return _diffuseColor; }
        RENDERING_API color getSpecularColor() const { return _specularColor; }
        RENDERING_API float getKa() const { return _ka; }
        RENDERING_API float getKd() const { return _kd; }
        RENDERING_API float getKs() const { return _ks; }
        RENDERING_API float getShininess() const { return _shininess; }

        RENDERING_API void setDiffuseTexture(texture* value) { _diffuseTexture = value; }
		RENDERING_API void setNormalTexture(texture* value) { _normalTexture = value; }
        RENDERING_API void setSpecularTexture(texture* value) { _specularTexture = value; }
        RENDERING_API void setAmbientColor(color value) { _ambientColor = value; }
        RENDERING_API void setDiffuseColor(color value) { _diffuseColor = value; }
        RENDERING_API void setSpecularColor(color value) { _specularColor = value; }
        RENDERING_API void setKa(float value) { _ka = value; }
		RENDERING_API void setKd(float value) { _kd = value; }
        RENDERING_API void setKs(float value) { _ks = value; }
        RENDERING_API void setShininess(float value) { _shininess = value; }
    };
}
#endif