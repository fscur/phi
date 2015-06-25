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

        _transform = new transform();
        updateViewMatrix();
        updateProjectionMatrix();
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
        updateProjectionMatrix();
	}

	void spotLight::setCutoff(float cutoff)
	{
		_cutoff = cutoff;
		_radius = calcRadius(_cutoff, _range);
		_boundingVolume->setRadius(_radius);
        updateProjectionMatrix();
	}

	void spotLight::setIntensity(float value)
	{
		_intensity = value;
	}

	void spotLight::update()
	{
		_boundingVolume->update();
	}

    void spotLight::updateViewMatrix()
    {
        glm::vec3 J = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 up = glm::normalize(J - _direction * glm::dot(_direction, J));

        _viewMatrix = glm::lookAt(_position, _position + _direction, up);
        _transform->setViewMatrix(_viewMatrix);
    }

    void spotLight::updateProjectionMatrix()
    {
        float angle = glm::acos(_cutoff) * 2.0f;
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