#include "transform.h"

#include <GLM\gtx\quaternion.hpp>

namespace phi
{
    mat4 transform::getLocalModelMatrix()
    {
        if (_changed)
            updateData();

        return _localModelMatrix;
    }

    mat4 transform::getLocalRotationMatrix()
    {
        return glm::toMat4(_localOrientation);
    }

    mat4 transform::getLocalTranslationMatrix()
    {
        return mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            _localPosition.x, _localPosition.y, _localPosition.z, 1.0f);
    }

    mat4 transform::getLocalScaleMatrix()
    {
        return mat4(
            _localSize.x, 0.0f, 0.0f, 0.0f,
            0.0f, _localSize.y, 0.0f, 0.0f,
            0.0f, 0.0f, _localSize.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }

    mat4 transform::getModelMatrix()
    {
        if (_parent == nullptr)
            return getLocalModelMatrix();
        else
            return _parent->getModelMatrix() * getLocalModelMatrix();
    }

    vec3 transform::getPosition()
    {
        if (_changed)
            updateData();

        return _position;
    }

    vec3 transform::getRight()
    {
        if (_changed)
            updateData();

        return _right;
    }

    vec3 transform::getUp()
    {
        if (_changed)
            updateData();
        
        return _up;
    }

    vec3 transform::getDirection()
    {
        if (_changed)
            updateData();

        return _direction;
    }

    void transform::setParent(transform* value)
    {
        _parent = value;
        _changed = true;
    }

    void transform::setLocalPosition(vec3 value)
    {
        _localPosition = value;
        _changed = true;
    }

    void transform::setLocalOrientation(quat value)
    {
        _localOrientation = value;
        _changed = true;
    }

    void transform::setLocalSize(vec3 value)
    {
        _localSize = value;
        _changed = true;
    }

    void transform::setDirection(vec3 direction)
    {
        setLocalOrientation(mathUtils::rotationBetweenVectors(_direction, direction) * _localOrientation);
    }

    void transform::updateData()
    {
        auto localRotation = getLocalRotationMatrix();
        auto localScale = getLocalScaleMatrix();
        auto localTranslation = getLocalTranslationMatrix();
        _localModelMatrix = localTranslation  * localRotation * localScale;
        _changed = false;

        auto rotation = localRotation;
        auto obj = _parent;

        while (obj != nullptr)
        {
            rotation = obj->getLocalRotationMatrix() * rotation;
            obj = obj->_parent;
        }

        _position = mathUtils::multiply(getModelMatrix(), vec3(0.0f, 0.0f, 0.0f));
        _right = mathUtils::multiply(rotation, vec3(1.0f, 0.0f, 0.0f));
        _up = mathUtils::multiply(rotation, vec3(0.0f, 1.0f, 0.0f));
        _direction = mathUtils::multiply(rotation, vec3(0.0f, 0.0f, 1.0f));
    }

    void transform::translate(vec3 translation)
    {
        setLocalPosition(_localPosition + translation);
    }

    void transform::rotate(float angle, vec3 axis)
    {
        setLocalOrientation(angleAxis(angle, axis) * _localOrientation);
    }

    void transform::pitch(float angle)
    {
        setLocalOrientation(angleAxis(angle, vec3(1.0f, 0.0f, 0.0f)) * _localOrientation);
    }

    void transform::yaw(float angle)
    {
        setLocalOrientation(angleAxis(angle, vec3(0.0f, 1.0f, 0.0f)) * _localOrientation);
    }

    void transform::roll(float angle)
    {
        setLocalOrientation(angleAxis(angle, vec3(0.0f, 0.0f, 1.0f)) * _localOrientation);
    }
}