#include <precompiled.h>
#include "directionalLight.h"
#include <rendering\shadersManager.h>

namespace phi
{
    directionalLight::directionalLight(string name, color color, float intensity, transform* transform) :
        light(componentType::DIRECTIONAL_LIGHT, name, color, intensity, transform)
    {
        _projectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -50, 50.0);
        updateViewMatrix();
    }

    directionalLight::~directionalLight()
    {
    }

    mat4 directionalLight::getViewMatrix()
    {
        if (_changed)
            updateViewMatrix();

        return _viewMatrix;
    }

    void directionalLight::updateViewMatrix()
    {
        auto direction = _transform->getDirection();
        vec3 J = vec3(0.0f, 1.0f, 0.0f);
        vec3 up = normalize(J - direction * dot(direction, J));

        auto position = _transform->getPosition();
        _viewMatrix = glm::lookAt(position, position + direction, up);
        _changed = false;
    }
}