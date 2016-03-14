#pragma once
#include <phi.h>
#include "scenes.h"
#include <core\color.h>
#include <core\component.h>

#include <rendering\attenuation.h>
#include <rendering\shader.h>
#include <rendering\texture.h>
#include <core\transform.h>

namespace phi
{
    //class transform;

    class light :
        public component
    {
    private:
        texture* _shadowMap;

    protected:
        color _color;
        float _intensity;
        transform* _transform;

    protected:
        light(component::componentType type, string name, color color, float intensity, transform* transform);

    public:
        SCENES_API virtual ~light();

        SCENES_API color getColor();
        SCENES_API float getIntensity();
        SCENES_API texture* getShadowMap();
        SCENES_API void setColor(color color);
        SCENES_API virtual void setIntensity(float intensity);
        SCENES_API void setShadowMap(texture* shadowMap);
        SCENES_API float calcRange(attenuation attenuation, color color);

        transform* getTransform() { return _transform; }
    };
}