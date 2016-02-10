#include "spotLight.h"

#include <rendering/shaderManager.h>

namespace phi
{
    spotLight::spotLight(vec3 position, color color, float intensity, float range, vec3 direction, float cutoff) :
        light(position, color, intensity, objectType::SPOT_LIGHT)
    {
        _cutoff = cutoff;
        _range = range;
        _oneOverRangeSqr = 1.0f / (pow(_range, 2.0f));
        _radius = calcRadius(cutoff, _range);

        _boundingVolume = cone::create(5, nullptr);
        auto diameter =  2.0f * _radius * 1.15f;
        setSize(sizef(diameter, diameter, _range));

        setLocalPosition(position);
        setDirection(normalize(direction));
        _transform = new transform();
        updateViewMatrix();
        updateProjectionMatrix();
    }

    spotLight::~spotLight()
    {
    }

    float spotLight::calcRadius(float cutoff, float range)
    {
        float angle = acos(cutoff);
        float tg = tan(angle);
        return range * tg * 2.0f;
    }

    void spotLight::setRange(float value)
    {
        _range = value;
        _oneOverRangeSqr = 1.0f / (pow(_range, 2.0f));
        _radius = calcRadius(_cutoff, _range);
        setSize(sizef(_radius, _radius, _range));
        updateProjectionMatrix();
    }

    void spotLight::setCutoff(float cutoff)
    {
        _cutoff = cutoff;
        _radius = calcRadius(_cutoff, _range);
        setSize(sizef(_radius, _radius, _range));
        updateProjectionMatrix();
    }

    void spotLight::setIntensity(float value)
    {
        _intensity = value;
    }

    void spotLight::update()
    {
        auto changed = getChanged();

        light::update();

        if (changed)
            updateViewMatrix();
    }

    void spotLight::updateViewMatrix()
    {
        auto direction = getDirection();
        vec3 J = vec3(0.0f, 1.0f, 0.0f);
        vec3 up = normalize(J - direction * dot(direction, J));

        _viewMatrix = lookAt(getPosition(), getPosition() + direction, up);
        _transform->setViewMatrix(_viewMatrix);
    }

    void spotLight::updateProjectionMatrix()
    {
        float angle = acos(_cutoff) * 2.0f;
        float nearPlane = 0.1f;
        float farPlane = _radius;
        _projectionMatrix = glm::perspective(angle, 1.0f, nearPlane, farPlane);
        _transform->setProjectionMatrix(_projectionMatrix);
    }

    transform* spotLight::getTransform()
    {
        if (!_transform)
        {
            _transform = new transform();
            updateViewMatrix();
            updateProjectionMatrix();
        }

        return _transform;
    }
}