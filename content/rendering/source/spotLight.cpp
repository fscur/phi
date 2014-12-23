#include "spotLight.h"
#include "shaderManager.h"

namespace phi
{
    spotLight::spotLight()
    {
    }

    spotLight::spotLight(glm::vec3 position, color color, float intensity, attenuation attenuation, glm::vec3 direction, float cutoff) :
        light(position, color, intensity)
    {
		_attenuation = attenuation;
        _direction = glm::normalize(direction);
        _cutoff = cutoff;
		_range = light::calcRange(attenuation, color);
		_radius = calcRadius(cutoff, _range);
		_boundingVolume = new cone(position, _range, _radius, 6, NULL);
		_boundingVolume->setDirection(direction);
    }

    spotLight::~spotLight()
    {}

	float spotLight::calcRadius(float cutoff, float range)
	{
		float angle = acos(cutoff);
		float tg = tan(angle);
		return range * tg * 2.15f;
	}

	void spotLight::setDirection(glm::vec3 direction)
	{
		_direction = direction; 
		_boundingVolume->setDirection(direction);
	}

	void spotLight::setAttenuation(attenuation attenuation)
    {
        _attenuation = attenuation;
		_range = light::calcRange(attenuation, _color);
		_radius = calcRadius(_cutoff, _range);
		_boundingVolume->setRadius(_range);
    }

	void spotLight::setCutoff(float cutoff)
	{
		_cutoff = cutoff;
		_radius = calcRadius(_cutoff, _range);
		_boundingVolume->setRadius(_radius);
	}

	void spotLight::update()
	{
		_boundingVolume->update();
	}
}