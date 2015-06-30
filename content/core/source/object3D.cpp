#include "object3D.h"

namespace phi
{
    object3D::object3D()
    {
        _position = glm::vec3(0.0f, 0.0f, 0.0f);
        _right = glm::vec3(1.0f, 0.0f, 0.0f);
        _up = glm::vec3(0.0f, 1.0f, 0.0f);
        _direction = glm::vec3(0.0f, 0.0f, 1.0f);
        _orientation = mathUtils::rotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), _direction);

        _yaw = 0;
        _roll = 0;
        _pitch = 0;

        _size = size<float>(1.0f, 1.0f, 1.0f);
        _changed = true;
        _aabb = nullptr;
    }

    object3D::~object3D()
    {
        DELETE(_aabb);
    }

    void object3D::setPosition(glm::vec3 value)
    {
        _position = value;
        _changed = true;
        onPositionChanged();
    }

    void object3D::setOrientation(glm::quat value)
    {
        _orientation = value;
        _changed = true;
    }

    void object3D::setDirection(glm::vec3 direction)
    {
        _orientation = mathUtils::rotationBetweenVectors(_direction, direction) * _orientation;
        _changed = true;
        onDirectionChanged();
    }

    void object3D::setSize(size<float> value)
    {
        _size = value;
        _changed = true;
    }

    glm::vec3 object3D::getDirection()
    {
        if (_changed)
            update();

        return _direction;
    }

    glm::vec3 object3D::getRight()
    {
        if (_changed)
            update();

        return _right;
    }

    glm::vec3 object3D::getUp()
    {
        if (_changed)
            update();

        return _up;
    }

    glm::mat4 object3D::getRotationMatrix()
    {
        return glm::toMat4(_orientation);
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
        _direction = _orientation * glm::vec3(0.0f, 0.0f, 1.0f);
        _right = _orientation * glm::vec3(1.0f, 0.0f, 0.0f);
        _up = _orientation * glm::vec3(0.0f, 1.0f, 0.0f);

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
        _orientation = glm::angleAxis(angle, axis) * _orientation;
        _changed = true;
    }

    void object3D::pitch(float angle)
    {
        _pitch += angle;
        _orientation = glm::angleAxis(angle, glm::vec3(1.0f, 0.0f, 0.0f)) * _orientation;
        _changed = true;
    }

    void object3D::yaw(float angle)
    {
        _yaw += angle;
        _orientation = glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f)) * _orientation;
        _changed = true;
    }

    void object3D::roll(float angle)
    {
        _roll += angle;
        _orientation = glm::angleAxis(angle, glm::vec3(0.0f, 0.0f, 1.0f)) * _orientation;
        _changed = true;
    }
}