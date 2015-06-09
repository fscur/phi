#include "object3D.h"

namespace phi
{
    object3D::object3D()
    {
        _position = glm::vec3(0.0f, 0.0f, 0.0f);
        _right = glm::vec3(1.0f, 0.0f, 0.0f);
        _up = glm::vec3(0.0f, 1.0f, 0.0f);
        _direction = glm::vec3(0.0f, 0.0f, 1.0f);

        _yaw = 0;
        _roll = 0;
        _pitch = 0;

        _size = size<float>(1.0f, 1.0f, 1.0f);
    }

    object3D::~object3D()
    {
    }

    void object3D::setDirection(glm::vec3 direction)
    {
        _direction = glm::normalize(direction);
        _right = glm::normalize(_right - _direction * glm::dot(_direction, _right));
        _up = glm::cross(_direction, _right);
        _changed = true;
    }

    glm::vec3 object3D::getDirection()
    {
        return _direction;
    }

    glm::vec3 object3D::getRight()
    {
        return _right;
    }

    glm::vec3 object3D::getUp()
    {
        return _up;
    }

    glm::mat4 object3D::getRotationMatrix()
    {
        return glm::toMat4(_quaternion);
    }

    glm::mat4 object3D::getTranslationMatrix()
    {
        return glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            _position.x, _position.y, _position.z, 1.0f);
    }

    glm::mat4 object3D::getScaleMatrix()
    {
        return glm::mat4(
            _size.width, 0.0f, 0.0f, 0.0f,
            0.0f, _size.height, 0.0f, 0.0f,
            0.0f, 0.0f, _size.depth, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    void object3D::update(glm::mat4 parentModelMatrix)
    {
        _direction = _quaternion * glm::vec3(1.0f, 0.0f, 0.0f);
        _right = _quaternion * glm::vec3(0.0f, 0.0f, 1.0f);
        _up = _quaternion * glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 rotation = getRotationMatrix();
        glm::mat4 scale = getScaleMatrix();
        glm::mat4 translation = getTranslationMatrix();

        _modelMatrix = parentModelMatrix * translation  * rotation * scale;

        glm::mat3 normalMatrix = glm::mat3(
            _modelMatrix[0][0], _modelMatrix[0][1], _modelMatrix[0][2],
            _modelMatrix[1][0], _modelMatrix[1][1], _modelMatrix[1][2],
            _modelMatrix[2][0], _modelMatrix[2][1], _modelMatrix[2][2]);
    }

    void object3D::update()
    {
        if (_changed)
        {
            update(glm::mat4(1.0f));
            _changed = false;
        }
    }

    void object3D::translate(glm::vec3 translation)
    {
        _position += translation;
        _changed = true;
    }

    void object3D::rotate(float angle, glm::vec3 axis)
    {
        _quaternion = glm::angleAxis(angle, axis) * _quaternion;
        _changed = true;
    }

    void object3D::pitch(float angle)
    {
        _pitch += angle;
        _quaternion = glm::angleAxis(glm::degrees(angle), glm::vec3(1.0f, 0.0f, 0.0f)) * _quaternion;
        _changed = true;
    }

    void object3D::yaw(float angle)
    {
        _yaw += angle;
        _quaternion = glm::angleAxis(glm::degrees(angle), glm::vec3(0.0f, 1.0f, 0.0f)) * _quaternion;
        _changed = true;
    }

    void object3D::roll(float angle)
    {
        _roll += angle;
        _quaternion = glm::angleAxis(glm::degrees(angle), glm::vec3(0.0f, 0.0f, 1.0f)) * _quaternion;
        _changed = true;
    }
}