#include "globals.h"
#include "light.h"

namespace phi
{
    light::light()
    {
        _position = glm::vec3(0.0f);
        _color = color::black;
        _intensity = 0.0f;
        _shadowMap = nullptr;
        _transform = new transform();
    }

    light::light(glm::vec3 position, color color, float intensity)
    {
        _position = position;
        _color = color;
        _intensity = intensity;
        _shadowMap = nullptr;
        _transform = new transform();
    }

    light::~light()
    {
        DELETE(_transform);
    }

    transform* light::getTransform()
    {
        if (!_transform)
            _transform = new transform();

        return _transform;
    }

    glm::vec3 light::getPosition()
    {
        return _position;
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

    void light::setPosition(glm::vec3 position)
    {
        _position = position;
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