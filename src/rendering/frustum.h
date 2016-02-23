#pragma once

#include <core/globals.h>
#include <core/size.h>
#include <core/eventHandler.h>

#include "rendering.h"

#include <iostream>
#include <vector>

namespace phi
{
    class frustum
    {
    private:
        vec3 _position;
        vec3 _direction;
        vec3 _up;
        vec3 _right;
        float _near;
        float _far;
        float _aspect;
        float _fov;
        mat4 _perspMatrix;
        vec2 _resolution;
        mat4 _perspectiveMatrix;
        bool _changed;

        vec3 _planeNormals[6];
        vec3 _planePositions[6];

        vec3 _topLeftNear;
        vec3 _topRightNear;
        vec3 _botLeftNear;
        vec3 _botRightNear;

        vec3 _topLeftFar;
        vec3 _topRightFar;
        vec3 _botLeftFar;
        vec3 _botRightFar;

    public:
        RENDERING_API frustum(
            vec3 position,
            vec3 direction,
            vec3 up,
            float near,
            float far,
            vec2 resolution,
            float fov);

        RENDERING_API ~frustum();

        RENDERING_API inline vec3 getPosition() const { return _position; }
        RENDERING_API inline vec3 getDirection() const { return _direction; }
        RENDERING_API inline vec3 getUp() const { return _up; }
        RENDERING_API inline float getZNear() const { return _near; }
        RENDERING_API inline float getZFar() const { return _far; }
        RENDERING_API inline float getAspect() const { return _aspect; }
        RENDERING_API inline vec2 getResolution() const { return _resolution; }
        RENDERING_API inline float getFov() const { return _fov; }
        RENDERING_API inline bool getChanged() const { return _changed; }
        RENDERING_API inline mat4 getPerspectiveMatrix() const { return _perspectiveMatrix; }

        RENDERING_API void setPosition(vec3 position);
        RENDERING_API void setDirection(vec3 direction);
        RENDERING_API void setUp(vec3 up);
        RENDERING_API void setZNear(float zNear);
        RENDERING_API void setZFar(float zFar);
        RENDERING_API void setAspect(float aspect);
        RENDERING_API void setResolution(vec2 resolution);
        RENDERING_API void setFov(float fov);

        RENDERING_API void update();

        RENDERING_API bool isInside(vec3 center, float radius);
    };
}
