#ifndef _PHI_LIGHT_H_
#define _PHI_LIGHT_H_

#include "color.h"
#include "mathUtils.h"
#include "Attenuation.h"
#include "Shader.h"
#include "texture.h"
#include "transform.h"
#include <string>

namespace phi
{
    class light
    {
    private:
        texture* _shadowMap;
    protected:
		color _color;
        float _intensity;
        glm::vec3 _position;
        transform* _transform;
        
    public:
        RENDERING_API light();
        RENDERING_API light(glm::vec3 position, color color, float intensity);
        virtual ~light();

        RENDERING_API virtual transform* getTransform();
        RENDERING_API glm::vec3 getPosition();
        RENDERING_API color getColor();
        RENDERING_API float getIntensity();
        RENDERING_API texture* getShadowMap();

        RENDERING_API virtual void setPosition(glm::vec3 position);
        RENDERING_API void setColor(color color);
        RENDERING_API void setIntensity(float intensity);
        RENDERING_API void setLightShader(shader* lightShader);
        RENDERING_API void setShadowMapShader(shader* shadowMapShader);
        RENDERING_API void setShadowMap(texture* shadowMap);

		RENDERING_API virtual void update(){};

		RENDERING_API float calcRange(attenuation attenuation, color color);
    };
}
#endif