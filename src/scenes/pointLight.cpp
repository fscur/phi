#include "phi/scenes/pointLight.h"

#include "phi/core/mathUtils.h"
#include "phi/rendering/shaderManager.h"

namespace phi
{
    pointLight::pointLight(glm::vec3 position, color color, float intensity, float range) :
        light(position, color, intensity)
    {
        _boundingVolumeSides = 5;
        _range = range;
        _oneOverRangeSqr = 1.0f / (glm::pow(_range, 2.0f));
        auto radius = calcRange(_range, _boundingVolumeSides);
        auto d = 2.0f * radius;
        setLocalPosition(position);
        setSize(size<float>(d, d, d));
        _boundingVolume = new sphere(radius, _boundingVolumeSides, _boundingVolumeSides, nullptr);
    }

    pointLight::~pointLight()
    {   
    }

    float pointLight::calcRange(float radius, unsigned int sides)
    {
        return (radius / glm::cos(PI/(float)sides)) * 1.1f;
    }

    void pointLight::setIntensity(float value)
    {
        _intensity = value;
    }

    void pointLight::setRange(float value)
    {
        _range = value;
        _oneOverRangeSqr = 1.0f / (glm::pow(_range, 2.0f));
        auto radius = calcRange(value, _boundingVolumeSides);
        _boundingVolume->setRadius(radius);
        setSize(size<float>(radius * 2.0, radius * 2.0, radius * 2.0));
    }
}