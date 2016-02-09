#ifndef _PHI_OBJECT_3D_H_
#define _PHI_OBJECT_3D_H_

#include "globals.h"
#include "core.h"
#include "size.h"
#include "mathUtils.h"
#include "aabb.h"
#include "eventHandler.h"
#include "object3DEventArgs.h"

#include <vector>

namespace phi
{
    class object3D
    {
    public:
        enum objectType
        {
            MODEL,
            MESH,
            CAMERA,
            DIRECTIONAL_LIGHT,
            SPOT_LIGHT,
            POINT_LIGHT
        };

    protected:
        std::string _name;
        objectType _type;

        vec3 _right;
        vec3 _up;
        vec3 _direction;
        vec3 _position;
        sizef _size;
        quat _orientation;
        mat4 _modelMatrix;
        aabb _aabb;

        object3D* _parent;
        std::vector<object3D*> _children;

        eventHandler<object3DEventArgs>* _changedEvent;
        bool _changed;

    private:
        void updateLocalModelMatrix();

    protected:
        CORE_API virtual void onChanged() { }
        CORE_API virtual void onPositionChanged() { }
        CORE_API virtual void onDirectionChanged() { }
        CORE_API void setChanged();
        CORE_API object3D(const object3D& original);

    public:
        CORE_API object3D(std::string name, objectType type);
        CORE_API ~object3D();

        CORE_API virtual object3D* clone();

        CORE_API inline std::string getName() const { return _name; }
        CORE_API inline objectType getType() const { return _type; }

        CORE_API vec3 getPosition();
        CORE_API mat4 getModelMatrix();
        CORE_API mat4 getRotationMatrix();
        CORE_API mat4 getTranslationMatrix();
        CORE_API mat4 getScaleMatrix();

        CORE_API inline vec3 getLocalPosition() const { return _position; }
        CORE_API inline sizef getSize() const { return _size; }
        CORE_API inline quat getOrientation() const { return _orientation; }
        CORE_API inline mat4 getLocalModelMatrix() const { return _modelMatrix; }
        CORE_API inline aabb getAabb() const { return _aabb; }
        CORE_API inline object3D* getParent() const { return _parent; }
        CORE_API inline vec3 getDirection() const { return _direction; }
        CORE_API inline vec3 getRight() const { return _right; }
        CORE_API inline vec3 getUp() const { return _up; }
        CORE_API inline std::vector<object3D*>& getChildren() { return _children; }
        CORE_API inline bool getChanged() const { return _changed; }
        CORE_API inline eventHandler<object3DEventArgs>* getChangedEvent() { return _changedEvent; }

        CORE_API void setOrientation(quat value);
        CORE_API void setLocalPosition(vec3 value);
        CORE_API void setSize(sizef value);
        CORE_API void setDirection(vec3 direction);
        CORE_API void setAabb(aabb value) { _aabb = value; }
        CORE_API void setParent(object3D* value) { _parent = value; setChanged(); }

        CORE_API void addChild(object3D* child);

        CORE_API virtual void update();

        CORE_API void translate(vec3 translation);
        CORE_API void rotate(float angle, vec3 axis);
        CORE_API void roll(float angle);
        CORE_API void pitch(float angle);
        CORE_API void yaw(float angle);
    };
}

#endif