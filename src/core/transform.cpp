#include <precompiled.h>
#include "transform.h"

#include <GLM\gtx\quaternion.hpp>

namespace phi
{
    transform::transform() :
        _parent(nullptr),
        _changed(false),
        _localSize(vec3(1.0f)),
        _localOrientation(quat()),
        _right(vec3(1.0f, 0.0f, 0.0f)),
        _up(vec3(0.0f, 1.0f, 0.0f)),
        _direction(vec3(0.0f, 0.0f, 1.0f)),
        _changedEvent(new eventHandler<>())
    {
    }

    transform::transform(const transform& original) :
        _localPosition(original._localPosition),
        _localSize(original._localSize),
        _localOrientation(original._localOrientation),
        _parent(original._parent),
        _changed(original._changed),
        _changedEvent(new eventHandler<>())
    {
    }

    transform::~transform()
    {
    }

    transform* transform::clone() const
    {
        return new transform(*this);
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

    mat4 transform::getLocalModelMatrix()
    {
        if (_changed)
            updateData();

        return _localModelMatrix;
    }

    mat4 transform::getLocalRotationMatrix() const
    {
        return glm::toMat4(_localOrientation);
    }

    mat4 transform::getLocalTranslationMatrix() const
    {
        return mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            _localPosition.x, _localPosition.y, _localPosition.z, 1.0f);
    }

    mat4 transform::getLocalScaleMatrix() const
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
        _changedEvent->raise();
    }

    void transform::setLocalPosition(vec3 value)
    {
        _localPosition = value;
        _changed = true;
        _changedEvent->raise();
    }

    void transform::setLocalOrientation(quat value)
    {
        _localOrientation = value;
        _changed = true;
        _changedEvent->raise();
    }

    void transform::setLocalSize(vec3 value)
    {
        _localSize = value;
        _changed = true;
        _changedEvent->raise();
    }

    void transform::setDirection(vec3 direction)
    {
        setLocalOrientation(mathUtils::rotationBetweenVectors(_direction, direction) * _localOrientation);
    }
}