#include "phi/scenes/light.h"
#include "phi/core/globals.h"

namespace phi
{
    light::light() :
        object3D()
    {
        _color = color::black;
        _intensity = 0.0f;
        _shadowMap = nullptr;
        _transform = new transform();
    }

    light::light(glm::vec3 position, color color, float intensity) :
        object3D()
    {
        setLocalPosition(position);
        _color = color;
        _intensity = intensity;
        _shadowMap = nullptr;
        _transform = new transform();
    }

    light::~light()
    {
        safeDelete(_transform);
    }

    transform* light::getTransform()
    {
        if (!_transform)
            _transform = new transform();

        return _transform;
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
        float maxColorChannel = mathUtils::max(mathUtils::max(color.r, color.g), color.b);
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