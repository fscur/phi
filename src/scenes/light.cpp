#include <precompiled.h>
#include "light.h"

namespace phi
{
    light::light(component::componentType type, string name, color color, float intensity) :
        component(type, name),
        _color(color),
        _intensity(intensity),
        _shadowMap(nullptr)
    {
    }

    light::~light()
    {
    }

    color light::getColor()
    {
        return _color;
    }

    float light::getIntensity()
    {
        return _intensity;
    }

    texture* light::getShadowMap()
    {
        return _shadowMap;
    }

    void light::setColor(color color)
    {
        _color = color;
    }

    void light::setIntensity(float intensity)
    {
        _intensity = intensity;
    }

    void light::setShadowMap(texture* shadowMap)
    {
        _shadowMap = shadowMap;
    }

    float light::calcRange(attenuation attenuation, color color)
    {
        float maxColorChannel = std::max(std::max(color.r, color.g), color.b);
        float linear = attenuation.linear;
        float exp = attenuation.exponential;
        float constant = attenuation.constant;

        // baskara
        // a = exp
        // b = linear
        // c = constant - 8 * maxColorChannel * _intensity

        float ret = (-linear + sqrtf(linear * linear - 4.0f * exp * (constant - 256.0f * maxColorChannel * _intensity))) / 2.0f * exp;

        return ret;
    }
}