#include "pointLight.h"
#include "shaderManager.h"
#include "mathUtils.h"

namespace phi
{
	pointLight::pointLight(glm::vec3 position, color color, float intensity, attenuation attenuation) :
		light(position, color, intensity)
	{
		_attenuation = attenuation;
		_range = light::calcRange(attenuation, color);
		_boundingVolume = new sphere(position, _range, 6, 6, nullptr);
	}

	pointLight::~pointLight()
	{   
	}

	attenuation pointLight::getAttenuation()
	{
		return _attenuation;
	}

	void pointLight::setAttenuation(attenuation attenuation)
	{
		_attenuation = attenuation;
		_range = light::calcRange(attenuation, _color);
		_boundingVolume->setRadius(_range);
	}

	void pointLight::setIntensity(float value)
	{
		_intensity = value;
		_range = light::calcRange(_attenuation, _color);
		_boundingVolume->setRadius(_range);
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