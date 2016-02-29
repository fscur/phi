#pragma once

#include "globals.h"
#include "mathUtils.h"

#include <GLM\glm.hpp>

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
        {}

        CORE_API transform* clone() { return new transform(*this); }

        CORE_API transform* getParent() { return _parent; }
        CORE_API vec3 getLocalPosition() { return _localPosition; }
        CORE_API quat getLocalOrientation() { return _localOrientation; }
        CORE_API vec3 getLocalSize() { return _localSize; }
        CORE_API mat4 getLocalModelMatrix();
        CORE_API mat4 getLocalRotationMatrix();
        CORE_API mat4 getLocalTranslationMatrix();
        CORE_API mat4 getLocalScaleMatrix();
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

        CORE_API void translate(vec3 translation);
        CORE_API void rotate(float angle, vec3 axis);
        CORE_API void pitch(float angle);
        CORE_API void yaw(float angle);
        CORE_API void roll(float angle);
    };
}