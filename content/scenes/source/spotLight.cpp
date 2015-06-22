#include "spotLight.h"
#include "shaderManager.h"

namespace phi
{
	spotLight::spotLight()
	{
	}

	spotLight::spotLight(glm::vec3 position, color color, float intensity, float range, glm::vec3 direction, float cutoff) :
		light(position, color, intensity)
	{
		_direction = glm::normalize(direction);
		_cutoff = cutoff;
		_range = range;
        _oneOverRangeSqr = 1.0f / (glm::pow(_range, 2.0f));
		_radius = calcRadius(cutoff, _range);
		_boundingVolume = new cone(_range, _radius * 1.15, 5, nullptr);
		_boundingVolume->setOrientation(direction);
        _boundingVolume->setPosition(position);
        _boundingVolume->update();
	}

	spotLight::~spotLight()
	{}

	float spotLight::calcRadius(float cutoff, float range)
	{
		float angle = acos(cutoff);
		float tg = tan(angle);
		return range * tg * 2.0f;
	}

	void spotLight::setDirection(glm::vec3 direction)
	{
		_direction = direction; 
		_boundingVolume->setOrientation(direction);
	}

	void spotLight::setRange(float value)
	{
		_range = value;
        _oneOverRangeSqr = 1.0f / (glm::pow(_range, 2.0f));
		_radius = calcRadius(_cutoff, _range);
		_boundingVolume->setRadius(_range);
	}

	void spotLight::setCutoff(float cutoff)
	{
		_cutoff = cutoff;
		_radius = calcRadius(_cutoff, _range);
		_boundingVolume->setRadius(_radius);
	}

	void spotLight::setIntensity(float value)
	{
		_intensity = value;
	}

	void spotLight::update()
	{
		_boundingVolume->update();
	}
}