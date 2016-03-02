#pragma once

#include "globals.h"
#include "mathUtils.h"

#include <GLM\glm.hpp>
#include <GLM\gtx\quaternion.hpp>

namespace phi
{
    class transform
    {
    private:
        transform* _parent;
        mat4 _localModelMatrix;
        vec3 _localPosition;
        quat _localOrientation;
        vec3 _localSize;
        vec3 _position;
        vec3 _right;
        vec3 _up;
        vec3 _direction;
        bool _changed;

    private:
        void updateData();

    public:
        transform() :
            _parent(nullptr),
            _changed(false),
            _localSize(vec3(1.0f)),
            _localOrientation(quat()),
            _right(vec3(1.0f, 0.0f, 0.0f)),
            _up(vec3(0.0f, 1.0f, 0.0f)),
            _direction(vec3(0.0f, 0.0f, 1.0f))
        {
        }

        transform(const transform& original) :
            _localPosition(original._localPosition),
            _localSize(original._localSize),
            _localOrientation(original._localOrientation),
            _parent(original._parent),
            _changed(original._changed)
        {
        }

        CORE_API transform* clone() const { return new transform(*this); }

        CORE_API transform* getParent() const { return _parent; }
        CORE_API vec3 getLocalPosition() const { return _localPosition; }
        CORE_API quat getLocalOrientation() const { return _localOrientation; }
        CORE_API vec3 getLocalSize() const { return _localSize; }
        CORE_API mat4 getLocalModelMatrix();
        CORE_API mat4 getLocalRotationMatrix() const;
        CORE_API mat4 getLocalTranslationMatrix() const;
        CORE_API mat4 getLocalScaleMatrix() const;
        CORE_API mat4 getModelMatrix();
        CORE_API vec3 getPosition();
        CORE_API vec3 getRight();
        CORE_API vec3 getUp();
        CORE_API vec3 getDirection();

        CORE_API void setParent(transform* value);
        CORE_API void setLocalPosition(vec3 value);
        CORE_API void setLocalOrientation(quat value);
        CORE_API void setLocalSize(vec3 value);
        CORE_API void setDirection(vec3 direction);

        CORE_API inline void translate(vec3 translation) { setLocalPosition(_localPosition + translation); }
        CORE_API inline void rotate(float angle, vec3 axis) { setLocalOrientation(angleAxis(angle, axis) * _localOrientation); }
        CORE_API inline void pitch(float angle) { setLocalOrientation(angleAxis(angle, vec3(1.0f, 0.0f, 0.0f)) * _localOrientation); }
        CORE_API inline void yaw(float angle) { setLocalOrientation(angleAxis(angle, vec3(0.0f, 1.0f, 0.0f)) * _localOrientation); }
        CORE_API inline void roll(float angle) { setLocalOrientation(angleAxis(angle, vec3(0.0f, 0.0f, 1.0f)) * _localOrientation); }
    };
}