#ifndef _PHI_FRUSTUM_H_
#define _PHI_FRUSTUM_H_

#include "rendering.h"
#include "eventHandler.h"
#include "glm\vec3.hpp"
#include "glm\mat4x4.hpp"
#include <iostream>
#include <vector>

namespace phi
{
    class frustum
    {
    private:
        glm::vec3 _position;
        glm::vec3 _direction;
        glm::vec3 _up;
        glm::vec3 _right;
        float _zNear;
        float _zFar;
        float _aspect;
        float _fov;
        glm::mat4 _perspMatrix;
        bool _changed;

        glm::vec3 _planeNormals[6];
        glm::vec3 _planePositions[6];

        glm::vec3 _topLeftNear;
        glm::vec3 _topRightNear;
        glm::vec3 _botLeftNear;
        glm::vec3 _botRightNear;

        glm::vec3 _topLeftFar;
        glm::vec3 _topRightFar;
        glm::vec3 _botLeftFar;
        glm::vec3 _botRightFar;

    public:
        RENDERING_API frustum(
            glm::vec3 position = glm::vec3(0.0f), 
            glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f), 
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
            float zNear = 0.01f, 
            float zFar = 1000.0f, 
            float aspect = 1.33f, 
            float fov = 0.78539816f);

        RENDERING_API ~frustum();

        RENDERING_API glm::vec3 getPosition() const { return _position; }
        RENDERING_API glm::vec3 getDirection() const { return _direction; }
        RENDERING_API glm::vec3 getUp() const { return _up; }
        RENDERING_API float getZNear() const { return _zNear; }
        RENDERING_API float getZFar() const { return _zFar; }
        RENDERING_API float getAspect() const { return _aspect; }
        RENDERING_API float getFov() const { return _fov; }
        RENDERING_API bool getChanged() const { return _changed; }

		RENDERING_API void setPosition(glm::vec3 position);
        RENDERING_API void setDirection(glm::vec3 direction);
        RENDERING_API void setUp(glm::vec3 up);
        RENDERING_API void setZNear(float zNear);
        RENDERING_API void setZFar(float zFar);
        RENDERING_API void setAspect(float aspect);
		RENDERING_API void setFov(float fov);

        RENDERING_API void debugRender();
        RENDERING_API void update();
        RENDERING_API glm::mat4 getPerspMatrix() const { return _perspMatrix; }
        RENDERING_API bool isInside(glm::vec3 center, float radius);
    };
}

#endif