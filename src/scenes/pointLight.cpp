#include <precompiled.h>
#include "pointLight.h"

#include <core\mathUtils.h>
#include <rendering\shadersManager.h>
#include <core\transform.h>

namespace phi
{
    pointLight::pointLight(string name, color color, float intensity, float range, transform* transform) :
        light(componentType::POINT_LIGHT, name, color, intensity, transform),
        _boundingVolumeSides(5),
        _range(range),
        _oneOverRangeSqr(1.0f / (pow(_range, 2.0f)))
    {
        auto radius = calcRange(_range, _boundingVolumeSides);
        auto d = 2.0f * radius;
        transform->setLocalSize(vec3(d, d, d));
        _boundingVolume = sphere::create(radius, _boundingVolumeSides, _boundingVolumeSides, nullptr);
    }

    pointLight::~pointLight()
    {
    }

    float pointLight::calcRange(float radius, unsigned int sides)
    {
        return (radius / cos(PI / (float)sides)) * 1.1f;
    }

    void pointLight::setIntensity(float value)
    {
        _intensity = value;
    }

    void pointLight::setRange(float value)
    {
        _range = value;
        _oneOverRangeSqr = 1.0f / (pow(_range, 2.0f));
        auto radius = calcRange(value, _boundingVolumeSides);
        _boundingVolume->setRadius(radius);
        _transform->setLocalSize(vec3(radius * 2.0f, radius * 2.0f, radius * 2.0f));
    }
}