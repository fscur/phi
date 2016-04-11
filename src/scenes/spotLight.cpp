#include <precompiled.h>
#include "spotLight.h"

#include <rendering\shadersManager.h>
#include <core\transform.h>

namespace phi
{
    spotLight::spotLight(string name, color color, float intensity, float range, float cutoff) :
        light(componentType::SPOT_LIGHT, name, color, intensity),
        _cutoff(cutoff),
        _range(range),
        _oneOverRangeSqr(1.0f / (pow(_range, 2.0f))),
        _radius(calcRadius(cutoff, _range)),
        _boundingVolume(cone::create(5, nullptr))
    {
        auto diameter =  2.0f * _radius * 1.15f;
        //transform->setLocalSize(vec3(diameter, diameter, _range)); TODO: fix this when implementing lights (not my problem)
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
        auto transform = getTransform();
        transform->setLocalSize(vec3(_radius, _radius, _range));
        updateProjectionMatrix();
    }

    void spotLight::setCutoff(float cutoff)
    {
        _cutoff = cutoff;
        _radius = calcRadius(_cutoff, _range);
        auto transform = getTransform();
        transform->setLocalSize(vec3(_radius, _radius, _range));
        updateProjectionMatrix();
    }

    void spotLight::setIntensity(float value)
    {
        _intensity = value;
    }

    void spotLight::updateViewMatrix()
    {
        auto transform = getTransform();
        auto direction = transform->getDirection();
        vec3 J = vec3(0.0f, 1.0f, 0.0f);
        vec3 up = normalize(J - direction * dot(direction, J));

        auto position = transform->getPosition();
        _viewMatrix = glm::lookAt(position, position + direction, up);
    }

    void spotLight::updateProjectionMatrix()
    {
        float angle = acos(_cutoff) * 2.0f;
        float nearPlane = 0.1f;
        float farPlane = _radius;
        _projectionMatrix = glm::perspective(angle, 1.0f, nearPlane, farPlane);
    }
}