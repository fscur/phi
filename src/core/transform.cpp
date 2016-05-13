#include <precompiled.h>
#include "transform.h"

#include <glm\gtx\quaternion.hpp>

namespace phi
{
    transform::transform() :
        _parent(nullptr),
        _localModelMatrix(mat4(1.0f)),
        _localPosition(vec3(0.0f)),
        _localOrientation(quat()),
        _localSize(vec3(1.0f)),
        _position(vec3(0.0f)),
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
		//_localModelMatrix = localScale * localRotation * localTranslation;
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

		//setLocalPosition(_localPosition + transform._localPosition);
		//setLocalSize(_localSize * transform._localSize);
		//setLocalOrientation(_localOrientation * transform._localOrientation);
	}
}