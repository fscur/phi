#ifndef _PHI_LIGHT_H_
#define _PHI_LIGHT_H_

#include <core\object3D.h>
#include <core\color.h>

#include <rendering\attenuation.h>
#include <rendering\shader.h>
#include <rendering\texture.h>
#include <rendering\transform.h>

#include "scenes.h"

#include <string>

namespace phi
{
    class light :
        public object3D
    {
    private:
        texture* _shadowMap;

    protected:
        color _color;
        float _intensity;
        transform* _transform;

    public:
        SCENES_API light(object3D::objectType type);
        SCENES_API light(vec3 position, color color, float intensity, object3D::objectType type);
        virtual ~light();

        SCENES_API virtual transform* getTransform();
        SCENES_API color getColor();
        SCENES_API float getIntensity();
        SCENES_API texture* getShadowMap();

        SCENES_API void setColor(color color);
        SCENES_API virtual void setIntensity(float intensity);
        SCENES_API void setLightShader(shader* lightShader);
        SCENES_API void setShadowMapShader(shader* shadowMapShader);
        SCENES_API void setShadowMap(texture* shadowMap);

        SCENES_API virtual void update() override { object3D::update(); };

        SCENES_API float calcRange(attenuation attenuation, color color);
    };
}
#endif