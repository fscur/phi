#include "frustum.h"

#include <glm/gtc/matrix_transform.hpp>

#if WIN32
#include <gl/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

#include <iostream>

namespace phi
{
    frustum::frustum(vec3 position, vec3 direction, vec3 up, float near, float far, sizef resolution, float fov) :
        _position(position),
        _direction(direction), 
        _up(up),
        _near(near),
        _far(far),
        _resolution(resolution),
        _aspect(resolution.w / resolution.h),
        _fov(fov),
        _perspectiveMatrix(glm::perspective(fov, _aspect, near, far)),
        _changed(true)
    {
        for (int i = 0; i < 6; ++i)
        {
            _planeNormals[i] = vec3(0.0f, 1.0f, 0.0f);
            _planePositions[i] = vec3(0.0f);
        }
    }

    frustum::~frustum()
    {
    }

    void frustum::setPosition(vec3 position)
    {
        _position = position;
        _changed = true;
    }

    void frustum::setDirection(vec3 direction)
    {
        _direction = direction;
        _changed = true;
    }

    void frustum::setUp(vec3 up)
    {
        _up = up;
        _changed = true;
    }

    void frustum::setZNear(float near)
    {
        _near = near;
        _perspectiveMatrix = glm::perspective(_fov, _aspect, _near, _far);
        _changed = true;
    }

    void frustum::setZFar(float far)
    {
        _far = far;
        _perspectiveMatrix = glm::perspective(_fov, _aspect, _near, _far);
        _changed = true;
    }

    void frustum::setAspect(float aspect)
    {
        _aspect = aspect;
        _perspMatrix = glm::perspective(_fov, _aspect, _near, _far);
        _changed = true;
    }

    void frustum::setResolution(sizef resolution)
    {
        _resolution = resolution;
        _aspect = resolution.w/resolution.h;
        _perspectiveMatrix = glm::perspective(_fov, _aspect, _near, _far);
        _changed = true;
    }

    void frustum::setFov(float fov)
    {
        _fov = fov;
        _perspectiveMatrix = glm::perspective(_fov, _aspect, _near, _far);
        _changed = true;
    }

    void frustum::update()
    {
        //if (!_changed)
            //return;
        _perspectiveMatrix = glm::perspective(_fov, _aspect, _near, _far);
        //_changed = true;

        auto tanHalfFov = tan(_fov * 0.5f);
        auto zNearWidth = tanHalfFov * _near;
        auto zNearHeight = zNearWidth * _fov;
        auto zFarWidth = tanHalfFov * _far;
        auto zFarHeight = zFarWidth * _fov;
        auto depth = _far - _near;
        auto halfDepth = depth * 0.5f;
        auto halfWidth = (zNearWidth + zFarWidth) * 0.5f;
        auto halfHeight = (zNearHeight + zFarHeight) * 0.5f;

        auto pos = _position;
        _right = cross(_direction, _up);

        _topLeftNear = _position + _direction * _near - _right * zNearWidth + _up * zNearHeight;
        _topRightNear = _position + _direction * _near + _right * zNearWidth + _up * zNearHeight;
        _botLeftNear = _position + _direction * _near - _right * zNearWidth - _up * zNearHeight;
        _botRightNear = _position + _direction * _near + _right * zNearWidth - _up * zNearHeight;

        _topLeftFar = _position + _direction * _far - _right * zFarWidth + _up * zFarHeight;
        _topRightFar = _position + _direction * _far + _right * zFarWidth + _up * zFarHeight;
        _botLeftFar = _position + _direction * _far - _right * zFarWidth - _up * zFarHeight;
        _botRightFar = _position + _direction * _far + _right * zFarWidth - _up * zFarHeight;

        //create near plane
        auto v1 = _botLeftNear - _botRightNear;
        auto v2 = _topLeftNear - _botRightNear;

        auto normal = normalize(cross(v1, v2));
        pos = _position + (_direction * _near);

        _planeNormals[0] = normal;
        _planePositions[0] = pos;

        //create right plane
        v1 = _topRightNear - _botRightNear;
        v2 = _botRightFar - _botRightNear;

        normal = normalize(cross(v1, v2));
        pos = _position + (_direction * (_near + halfDepth)) + _right * halfWidth;

        _planeNormals[1] = normal;
        _planePositions[1] = pos;

        //create top plane
        v1 = _topLeftNear - _topRightNear;
        v2 = _topRightFar - _topRightNear;

        normal = normalize(cross(v1, v2));
        pos = _position + (_direction * (_near + halfDepth)) + _up * halfHeight;

        _planeNormals[2] = normal;
        _planePositions[2] = pos;

        //create left plane
        v1 = _botLeftFar - _botLeftNear;
        v2 = _topLeftNear - _botLeftNear;

        normal = normalize(cross(v1, v2));
        pos = _position + (_direction * (_near + halfDepth)) - _right * halfWidth;

        _planeNormals[3] = normal;
        _planePositions[3] = pos;

        //create bot plane
        v1 = _botRightFar - _botRightNear;
        v2 = _botLeftNear - _botRightNear;

        normal = normalize(cross(v1, v2));
        pos = _position + (_direction * (_near + halfDepth)) - _up * halfHeight;

        _planeNormals[4] = normal;
        _planePositions[4] = pos;

        //create far plane
        v1 = _botRightFar - _botLeftFar;
        v2 = _topRightFar - _botLeftFar;

        normal = normalize(cross(v1, v2));
        pos = _position + (_direction * _far);

        _planeNormals[5] = normal;
        _planePositions[5] = pos;

        _changed = false;
    }

    bool frustum::isInside(vec3 center, float radius)
    {
        vec3 normal;
        vec3 pos;
        float dist;

        for (unsigned int i = 0; i < 6; ++i)
        {
            //plane X bounding-sphere intersection
            //plane normal
            normal = _planeNormals[i];
            //object position in relation to plane
            pos = center - _planePositions[i];
            // projection of object position vector into plane normal vector, 
            // a positive value means the object is in the same side of plane's normal
            dist = dot(normal, pos);

            if (dist + radius < 0)
                return false;
        }

        return true;
    }
}