#ifndef _PHI_OBJECT_3D_H_
#define _PHI_OBJECT_3D_H_

#include "globals.h"
#include "core.h"
#include "size.h"
#include "mathUtils.h"
#include "aabb.h"
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

    protected:
        bool _changed;

    private:
        void update(glm::mat4 transform);

    protected:
        CORE_API virtual void onPositionChanged() { }
        CORE_API virtual void onDirectionChanged() { }

    public:
        CORE_API glm::vec3 getPosition() const { return _position; }
        CORE_API size<float> getSize() const { return _size ; }
        CORE_API glm::quat getOrientation() const { return _orientation; }
        CORE_API glm::mat4 getModelMatrix() const { return _modelMatrix; }
        CORE_API glm::mat4 getRotationMatrix();
        CORE_API glm::mat4 getTranslationMatrix();
        CORE_API glm::mat4 getScaleMatrix();
        CORE_API aabb* getAabb() const { return _aabb; }

        CORE_API glm::vec3 getDirection();
        CORE_API glm::vec3 getRight();
        CORE_API glm::vec3 getUp();

        CORE_API void setOrientation(glm::quat value);
        CORE_API void setPosition(glm::vec3 value);
        CORE_API void setSize(size<float> value);
        CORE_API void setDirection(glm::vec3 direction);
        CORE_API void setAabb(aabb* value) { _aabb = value; }

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