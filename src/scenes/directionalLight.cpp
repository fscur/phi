#include "directionalLight.h"

#include <rendering\shaderManager.h>
#include <GLM\gtc\matrix_transform.hpp>

namespace phi
{
    directionalLight::directionalLight(vec3 position, color color, float intensity, vec3 direction) :
        light(position, color, intensity, objectType::DIRECTIONAL_LIGHT)
    {
        setDirection(direction);
        _projectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -50, 50.0);
        updateViewMatrix();
    }

    directionalLight::~directionalLight()
    {
    }

    void directionalLight::updateViewMatrix()
    {
        auto direction = getDirection();
        vec3 J = vec3(0.0f, 1.0f, 0.0f);
        vec3 up = normalize(J - direction * dot(direction, J));

        _viewMatrix = glm::lookAt(getPosition(), getPosition() + getDirection(), up);
        _transform->setViewMatrix(_viewMatrix);
    }

    void directionalLight::update()
    {
        auto changed = getChanged();

        light::update();

        if (changed)
            updateViewMatrix();
    }
}