#ifndef _PHI_OBJECT_3D_H_
#define _PHI_OBJECT_3D_H_

#include "globals.h"
#include "core.h"
#include "size.h"
#include "mathUtils.h"
#include "aabb.h"
#include "eventHandler.h"
#include "object3D.h"
#include "object3DEventArgs.h"
#include <vector>

namespace phi
{
    class object3D
    {
    private:
        float _roll;
        float _pitch;
        float _yaw;
        glm::vec3 _right;
        glm::vec3 _up;
        glm::vec3 _direction;
        glm::vec3 _position;
        size<float> _size;
        glm::quat _orientation;
        glm::mat4 _modelMatrix;
        aabb* _aabb;
        std::vector<object3D*> _children;
        object3D* _parent;
        eventHandler<object3DEventArgs>* _changedEvent;

    protected:
        bool _changed;

    private:
        void updateLocalModelMatrix();
        void setChanged(bool value);

    protected:
        CORE_API virtual void onChanged() {}
        CORE_API virtual void onPositionChanged() { }
        CORE_API virtual void onDirectionChanged() { }

    public:
        CORE_API glm::vec3 getPosition();
        CORE_API glm::vec3 getLocalPosition();
        CORE_API size<float> getSize() const { return _size ; }
        CORE_API glm::quat getOrientation() const { return _orientation; }
        CORE_API glm::mat4 getModelMatrix();
        CORE_API glm::mat4 getLocalModelMatrix() const { return _modelMatrix; }
        CORE_API glm::mat4 getRotationMatrix();
        CORE_API glm::mat4 getTranslationMatrix();
        CORE_API glm::mat4 getScaleMatrix();
        CORE_API aabb* getAabb() const { return _aabb; }
        CORE_API object3D* getParent() const { return _parent; }
        CORE_API glm::vec3 getDirection();
        CORE_API glm::vec3 getRight();
        CORE_API glm::vec3 getUp();
        CORE_API std::vector<object3D*> getChildren() const { return _children; }
        CORE_API bool getChanged() const { return _changed; }
        CORE_API eventHandler<object3DEventArgs>* getChangedEvent() { return _changedEvent; }

        CORE_API void setOrientation(glm::quat value);
        CORE_API void setLocalPosition(glm::vec3 value);
        CORE_API void setSize(size<float> value);
        CORE_API void setDirection(glm::vec3 direction);
        CORE_API void setAabb(aabb* value) { _aabb = value; }
        CORE_API void setParent(object3D* value) { _parent = value; setChanged(true); }

        CORE_API void addChild(object3D* child);

        CORE_API object3D();
        CORE_API ~object3D();

        CORE_API virtual void update();

        CORE_API void translate(glm::vec3 translation);
        CORE_API void rotate(float angle, glm::vec3 axis);
        CORE_API void roll(float angle);
        CORE_API void pitch(float angle);
        CORE_API void yaw(float angle);
    };
}

#endif