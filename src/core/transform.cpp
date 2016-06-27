#include <precompiled.h>
#include "transform.h"

namespace phi
{
    transform::transform() :
        _parent(nullptr),
        _localModelMatrix(mat4(1.0f)),
        _localPosition(vec3(0.0f)),
        _localOrientation(quat()),
        _localSize(vec3(1.0f)),
        _position(_localPosition),
        _orientation(_localOrientation),
        _size(_localSize),
        _right(vec3(1.0f, 0.0f, 0.0f)),
        _up(vec3(0.0f, 1.0f, 0.0f)),
        _direction(vec3(0.0f, 0.0f, 1.0f)),
        _changed(false),
        _changedEvent(new eventHandler<transform*>()),
        _parentTransformChangedEventToken(eventToken())
    {
    }

    transform::transform(const transform& original) :
        _parent(original._parent),
        _localModelMatrix(original._localModelMatrix),
        _localPosition(original._localPosition),
        _localOrientation(original._localOrientation),
        _localSize(original._localSize),
        _position(original._position),
        _right(original._right),
        _up(original._up),
        _direction(original._direction),
        _changed(original._changed),
        _changedEvent(new eventHandler<transform*>()),
        _parentTransformChangedEventToken(eventToken())
    {
        if (_parent)
            _parentTransformChangedEventToken = _parent->getChangedEvent()->assign(std::bind(&transform::parentTransformChanged, this, std::placeholders::_1));
    }

    transform::~transform()
    {
        if (_parent)
            _parent->getChangedEvent()->unassign(_parentTransformChangedEventToken);

        safeDelete(_changedEvent);
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

        if (_parent)
        {
            auto parentOrientation = _parent->getOrientation();
            _position = _parent->getPosition() + parentOrientation * _localPosition;
            _orientation = parentOrientation * _localOrientation;
            _size = _parent->getSize() * _localSize;
        }
        else
        {
            _position = _localPosition;
            _orientation = _localOrientation;
            _size = _localSize;
        }

        _right = _orientation * vec3(1.0f, 0.0f, 0.0f);
        _up = _orientation * vec3(0.0f, 1.0f, 0.0f);
        _direction = _orientation *  vec3(0.0f, 0.0f, 1.0f);
    }

    void transform::setChanged()
    {
        _changed = true;
        _changedEvent->raise(this);
    }

    void transform::parentTransformChanged(transform* transform)
    {
        setChanged();
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
        // TODO: we can probably cache this model matrix
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

    quat transform::getOrientation()
    {
        if (_changed)
            updateData();

        return _orientation;
    }

    vec3 transform::getSize()
    {
        if (_changed)
            updateData();

        return _size;
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
        if (_parent)
            _parent->getChangedEvent()->unassign(_parentTransformChangedEventToken);

        _parent = value;

        if (_parent)
            _parentTransformChangedEventToken = _parent->getChangedEvent()->assign(std::bind(&transform::parentTransformChanged, this, std::placeholders::_1));

        setChanged();
    }

    void transform::setLocalPosition(vec3 value)
    {
        _localPosition = value;
        setChanged();
    }

    void transform::setLocalOrientation(quat value)
    {
        _localOrientation = value;
        setChanged();
    }

    void transform::setLocalSize(vec3 value)
    {
        _localSize = value;
        setChanged();
    }

    void transform::setDirection(vec3 direction)
    {
        setLocalOrientation(mathUtils::rotationBetweenVectors(_direction, direction) * _localOrientation);
    }

    inline void transform::translate(vec3 translation)
    {
        setLocalPosition(_localPosition + translation);
    }

    inline void transform::rotate(float angle, vec3 axis)
    {
        setLocalOrientation(angleAxis(angle, axis) * _localOrientation);
    }

    inline void transform::pitch(float angle) { setLocalOrientation(angleAxis(angle, vec3(1.0f, 0.0f, 0.0f)) * _localOrientation); }
    inline void transform::yaw(float angle) { setLocalOrientation(angleAxis(angle, vec3(0.0f, 1.0f, 0.0f)) * _localOrientation); }
    inline void transform::roll(float angle) { setLocalOrientation(angleAxis(angle, vec3(0.0f, 0.0f, 1.0f)) * _localOrientation); }

    void transform::multiply(const transform & transform)
    {
        _localPosition += transform._localPosition;
        _localSize *= transform._localSize;
        _localOrientation *= transform._localOrientation;
        setChanged();
    }

    bool transform::operator==(transform& other)
    {
        if (_parent && other._parent)
        {
            if (*_parent != *other._parent)
                return false;
        }
        else if (_parent != other._parent)
        {
            return false;
        }

        return
            getModelMatrix() == other.getModelMatrix() &&
            _localModelMatrix == other._localModelMatrix &&
            _localOrientation == other._localOrientation &&
            _localPosition == other._localPosition &&
            _localSize == other._localSize &&
            _position == other._position &&
            _direction == other._direction &&
            _right == other._right &&
            _up == other._up &&
            _changed == other._changed;
    }

    bool transform::operator!=(transform& other)
    {
        return !this->operator==(other);
    }
}