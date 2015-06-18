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
        _direction = glm::normalize(direction);
        updateViewMatrix();
    }

    directionalLight::~directionalLight()
    {}

    glm::vec3 directionalLight::getDirection()
    {
        return _direction;
    }

    void directionalLight::updateViewMatrix()
    {
        glm::vec3 J = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 up = glm::normalize(J - _direction * glm::dot(_direction, J));

        _viewProjectionMatrix = glm::lookAt(_position, _position + _direction, up);
        _transform->setViewMatrix(_viewProjectionMatrix);
    }

    void directionalLight::setDirection(glm::vec3 direction)
    {
        _direction = glm::normalize(direction);
        updateViewMatrix();
    }

    void directionalLight::setPosition(glm::vec3 position)
    {
        _position = position;
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