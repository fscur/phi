#ifndef _PHI_OBJECT_3D_H_
#define _PHI_OBJECT_3D_H_

#include "core.h"
#include "size.h"
#include "mathUtils.h"
#include "aabb.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

namespace phi
{
    class object3D
    {
    private:
        float _roll;
        float _pitch;
        float _yaw;

    protected:
        size<float> _size;
        glm::vec3 _position;
        glm::quat _quaternion;
        glm::mat4 _modelMatrix;
        aabb* _aabb;
        bool _changed;

        glm::vec3 _right;
        glm::vec3 _up;
        glm::vec3 _direction;

    private:
        void update(glm::mat4 transform);

    public:
        CORE_API glm::vec3 getPosition() const { return _position; }
        CORE_API size<float> getSize() const { return _size ; }
        CORE_API glm::quat getQuaternion() const { return _quaternion; }
        CORE_API glm::mat4 getModelMatrix() const { return _modelMatrix; }
        CORE_API glm::mat4 getRotationMatrix();
        CORE_API glm::mat4 getTranslationMatrix();
        CORE_API glm::mat4 getScaleMatrix();
        CORE_API aabb* getAabb() const { return _aabb; }

        CORE_API void setPosition(glm::vec3 value) { _position = value; _changed = true; }
        CORE_API void setSize(size<float> value) { _size = value; _changed = true; }
        CORE_API void setDirection(glm::vec3 direction);
        CORE_API void setAabb(aabb* value) { _aabb = value; };

        CORE_API glm::vec3 getDirection();
        CORE_API glm::vec3 getRight();
        CORE_API glm::vec3 getUp();

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