#pragma once
#include <phi.h>
#include "core.h"
#include "mathUtils.h"
#include "eventHandler.h"

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
        eventHandler<>* _changedEvent;

    private:
        void updateData();

    public:
        CORE_API transform();
        CORE_API transform(const transform& original);
        CORE_API ~transform();
        CORE_API transform* clone() const;
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

        transform* getParent() const { return _parent; }
        vec3 getLocalPosition() const { return _localPosition; }
        quat getLocalOrientation() const { return _localOrientation; }
        vec3 getLocalSize() const { return _localSize; }
        eventHandler<>* getChangedEvent() const { return _changedEvent; }

        void translate(vec3 translation) { setLocalPosition(_localPosition + translation); }
        void rotate(float angle, vec3 axis) { setLocalOrientation(angleAxis(angle, axis) * _localOrientation); }
        void pitch(float angle) { setLocalOrientation(angleAxis(angle, vec3(1.0f, 0.0f, 0.0f)) * _localOrientation); }
        void yaw(float angle) { setLocalOrientation(angleAxis(angle, vec3(0.0f, 1.0f, 0.0f)) * _localOrientation); }
        void roll(float angle) { setLocalOrientation(angleAxis(angle, vec3(0.0f, 0.0f, 1.0f)) * _localOrientation); }
    };
}