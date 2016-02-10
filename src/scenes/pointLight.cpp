#include "pointLight.h"

#include <core/mathUtils.h>
#include <rendering/shaderManager.h>

namespace phi
{
    pointLight::pointLight(vec3 position, color color, float intensity, float range) :
        light(position, color, intensity, objectType::POINT_LIGHT)
    {
        _boundingVolumeSides = 5;
        _range = range;
        _oneOverRangeSqr = 1.0f / (pow(_range, 2.0f));
        auto radius = calcRange(_range, _boundingVolumeSides);
        auto d = 2.0f * radius;
        setLocalPosition(position);
        setSize(sizef(d, d, d));
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
        setSize(sizef(radius * 2.0f, radius * 2.0f, radius * 2.0f));
    }
}