#include "pointLight.h"
#include "shaderManager.h"
#include "mathUtils.h"

namespace phi
{
	pointLight::pointLight(glm::vec3 position, color color, float intensity, float range) :
		light(position, color, intensity)
	{
        _boundingVolumeSides = 5;
		_range = range;
        _oneOverRangeSqr = 1.0f / (glm::pow(_range, 2.0f));
		_boundingVolume = new sphere(position, calcRange(_range, _boundingVolumeSides), _boundingVolumeSides, _boundingVolumeSides, nullptr);
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
		_boundingVolume->setRadius(calcRange(value, _boundingVolumeSides));
    }

	void pointLight::setPosition(glm::vec3 position)
	{
		light::setPosition(position);
		_boundingVolume->setPosition(position);
	}

	void pointLight::update()
	{
		_boundingVolume->update();
	}
}