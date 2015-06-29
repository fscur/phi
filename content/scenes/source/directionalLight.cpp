#include "directionalLight.h"
#include "shaderManager.h"

namespace phi
{
    directionalLight::directionalLight() : 
        light()
    {   
    }

    directionalLight::directionalLight(glm::vec3 position, color color, float intensity, glm::vec3 direction) :
        light(position, color, intensity)
    {
        setDirection(direction);
    }

    directionalLight::~directionalLight()
    {
    }

    void directionalLight::updateViewMatrix()
    {
        auto direction = getDirection();
        glm::vec3 J = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 up = glm::normalize(J - direction * glm::dot(direction, J));

        _viewProjectionMatrix = glm::lookAt(getPosition(), getPosition() + direction, up);
        _transform->setViewMatrix(_viewProjectionMatrix);
    }

    void directionalLight::onDirectionChanged()
    {
        updateViewMatrix();
    }

    void directionalLight::onPositionChanged()
    {
        updateViewMatrix();
    }

    transform* directionalLight::getTransform()
    {
        if (!_transform)
        {
            _transform = new transform();
            updateViewMatrix();
            _orthoProjectionMatrix = glm::ortho<float>(-20, 20, -20, 20, -50, 50.0);
            _transform->setProjectionMatrix(_orthoProjectionMatrix);
        }

        return _transform;
    }
}