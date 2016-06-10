#pragma once
#include <phi.h>
#include "coreApi.h"
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
        quat _orientation;
        vec3 _size;
        vec3 _right;
        vec3 _up;
        vec3 _direction;
        bool _changed;
        eventHandler<transform*>* _changedEvent;
        eventToken _parentTransformChangedEventToken;

    private:
        void updateData();
        void setChanged();
        void parentTransformChanged(transform* sender);

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
        CORE_API quat getOrientation();
        CORE_API vec3 getSize();
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
        eventHandler<transform*>* getChangedEvent() const { return _changedEvent; }

        CORE_API void translate(vec3 translation);
        CORE_API void rotate(float angle, vec3 axis);
        CORE_API void pitch(float angle);
        CORE_API void yaw(float angle);
        CORE_API void roll(float angle);
        CORE_API void multiply(const transform& transform);

        CORE_API bool operator ==(transform& other);
        CORE_API bool operator !=(transform& other);
    };
}