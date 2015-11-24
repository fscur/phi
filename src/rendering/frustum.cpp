#include <phi/rendering/frustum.h>

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#if WIN32
#include <gl/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

namespace phi
{
    frustum::frustum(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float zNear, float zFar, float aspect, float fov)
    {
        _position = position;
        _direction = direction;
        _up = up;
        _zNear = zNear;
        _zFar = zFar;
        _aspect = aspect;
        _fov  = fov;
        _perspMatrix = glm::perspective(fov, aspect, _zNear, _zFar);

        for (int i = 0; i < 6; i++)
        {
            _planeNormals[i] = glm::vec3(0.0f, 1.0f, 0.0f);
            _planePositions[i] = glm::vec3(0.0f);
        }

        _changed = true;
    }

    frustum::~frustum()
    {
    }

	void frustum::setPosition(glm::vec3 position) 
    { 
        _position = position;
        _changed = true;
    }

	void frustum::setDirection(glm::vec3 direction)
    {
        _direction = direction; 
        _changed = true;
    }

	void frustum::setUp(glm::vec3 up)
    { 
        _up = up; 
        _changed = true;
    }

	void frustum::setZNear(float zNear) 
    {
        _zNear = zNear; 
        _perspMatrix = glm::perspective(_fov, _aspect, _zNear, _zFar);
        _changed = true;
    }

	void frustum::setZFar(float zFar) 
    { 
        _zFar = zFar; 
        _perspMatrix = glm::perspective(_fov, _aspect, _zNear, _zFar);
        _changed = true;
    }

	void frustum::setAspect(float aspect) 
    { 
        _aspect = aspect; 
        _perspMatrix = glm::perspective(_fov, _aspect, _zNear, _zFar);
        _changed = true;
    }

	void frustum::setFov(float fov) 
    { 
        _fov = fov; 
        _perspMatrix = glm::perspective(_fov, _aspect, _zNear, _zFar);
        _changed = true;
    }

    /*
    void frustum::Update()
    {
        if (!_changed)
            return;

        float tanHalfFov = tan(_fov/2);
        float zNearWidth = tanHalfFov * _zNear;
        float zNearHeight = zNearWidth * _fov;
        float zFarWidth = tanHalfFov * _zFar;
        float zFarHeight = zFarWidth * _fov;
        float halfZNearWidth = zNearWidth / 2.0f;
        float halfZFarWidth = zFarWidth / 2.0f;
        float len = _zFar-_zNear;

        glm::vec3 pos = _position;
        _right = glm::cross(_direction, _up);

        glm::vec3 topLeftNear = _position + _direction * _zNear - _right * zNearWidth + _up * zNearHeight;
        glm::vec3 topRightNear = _position + _direction * _zNear + _right * zNearWidth + _up * zNearHeight;
        glm::vec3 botLeftNear = _position + _direction * _zNear - _right * zNearWidth - _up * zNearHeight;
        glm::vec3 botRightNear = _position + _direction * _zNear + _right * zNearWidth - _up * zNearHeight;

        glm::vec3 topLeftFar = _position + _direction * _zFar - _right * zFarWidth + _up * zFarHeight;
        glm::vec3 topRightFar = _position + _direction * _zFar + _right * zFarWidth + _up * zFarHeight;
        glm::vec3 botLeftFar = _position + _direction * _zFar - _right * zFarWidth - _up * zFarHeight;
        glm::vec3 botRightFar = _position + _direction * _zFar + _right * zFarWidth - _up * zFarHeight;

        for (int i = 0; i < 6; i++)
        {
            if (_planes[i] != NULL)
                delete _planes[i];
        }

        //create near plane
        glm::vec3 v1 = botLeftNear - botRightNear;
        glm::vec3 v2 = topLeftNear - botRightNear;

        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * _zNear);

        _planeNormals[0] = normal;
        _planePositions[0] = pos;

        _planes[0] = new Plane(normal, pos, Size<float>(zNearWidth, 0.0f, zNearHeight), ResourceManager::Get()->GetMaterial("Red"));
        _planes[0]->AddPoint(botLeftNear);
        _planes[0]->AddPoint(botRightNear);
        _planes[0]->AddPoint(topRightNear);
        _planes[0]->AddPoint(topLeftNear);

        //create right plane
        v1 = topRightNear - botRightNear;
        v2 = botRightFar - botRightNear;

        normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * (_zNear + (len/ 2.0f))) + _right * ((zNearWidth + zFarWidth)/2.0f);

        _planeNormals[1] = normal;
        _planePositions[1] = pos;

        _planes[1] = new Plane(normal, pos, Size<float>(len, 0.0f, len), ResourceManager::Get()->GetMaterial("White"));
        _planes[1]->AddPoint(botRightNear);
        _planes[1]->AddPoint(botRightFar);
        _planes[1]->AddPoint(topRightFar);
        _planes[1]->AddPoint(topRightNear);

        //create top plane
        v1 = topLeftNear - topRightNear;
        v2 = topRightFar - topRightNear;

        normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * (_zNear + (len / 2.0f))) + _up * ((zNearHeight + zFarHeight)/2.0f);

        _planeNormals[2] = normal;
        _planePositions[2] = pos;

        _planes[2] = new Plane(normal, pos, Size<float>(len, 0.0f, len), ResourceManager::Get()->GetMaterial("White"));
        _planes[2]->AddPoint(topRightFar);
        _planes[2]->AddPoint(topLeftFar);
        _planes[2]->AddPoint(topLeftNear);
        _planes[2]->AddPoint(topRightNear);

        //create left plane
        v1 = botLeftFar - botLeftNear;
        v2 = topLeftNear - botLeftNear;

        normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * (_zNear + (len / 2.0f))) - _right * ((zNearWidth + zFarWidth)/2.0f);

        _planeNormals[3] = normal;
        _planePositions[3] = pos;

        _planes[3] = new Plane(normal, pos, Size<float>(len, 0.0f, len), ResourceManager::Get()->GetMaterial("White"));
        _planes[3]->AddPoint(botLeftFar);
        _planes[3]->AddPoint(botLeftNear);
        _planes[3]->AddPoint(topLeftNear);
        _planes[3]->AddPoint(topLeftFar);

        //create bot plane
        v1 = botRightFar - botRightNear;
        v2 = botLeftNear - botRightNear;

        normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * (_zNear + (len / 2.0f))) - _up * ((zNearHeight + zFarHeight)/2.0f);

        _planeNormals[4] = normal;
        _planePositions[4] = pos;

        _planes[4] = new Plane(normal, pos, Size<float>(len, 0.0f, len), ResourceManager::Get()->GetMaterial("White"));
        _planes[4]->AddPoint(botLeftFar);
        _planes[4]->AddPoint(botRightFar);
        _planes[4]->AddPoint(botRightNear);
        _planes[4]->AddPoint(botLeftNear);

        //create far plane
        v1 = botRightFar - botLeftFar;
        v2 = topRightFar - botLeftFar;

        normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * _zFar);

        _planeNormals[5] = normal;
        _planePositions[5] = pos;

        _planes[5] = new Plane(normal, pos, Size<float>(zFarWidth, 0.0f, zFarHeight), ResourceManager::Get()->GetMaterial("Red"));;
        _planes[5]->AddPoint(botRightFar);
        _planes[5]->AddPoint(botLeftFar);
        _planes[5]->AddPoint(topLeftFar);
        _planes[5]->AddPoint(topRightFar);
    }
    */

    void frustum::update()
    {
        //if (!_changed)
            //return;

        float tanHalfFov = tan(_fov * 0.5f);
        float zNearWidth = tanHalfFov * _zNear;
        float zNearHeight = zNearWidth * _fov;
        float zFarWidth = tanHalfFov * _zFar;
        float zFarHeight = zFarWidth * _fov;
        float depth = _zFar-_zNear;
        float halfDepth = depth * 0.5f;
        float halfWidth = (zNearWidth + zFarWidth) * 0.5f;
        float halfHeight = (zNearHeight + zFarHeight) * 0.5f;

        glm::vec3 pos = _position;
        _right = glm::cross(_direction, _up);

        _topLeftNear = _position + _direction * _zNear - _right * zNearWidth + _up * zNearHeight;
        _topRightNear = _position + _direction * _zNear + _right * zNearWidth + _up * zNearHeight;
        _botLeftNear = _position + _direction * _zNear - _right * zNearWidth - _up * zNearHeight;
        _botRightNear = _position + _direction * _zNear + _right * zNearWidth - _up * zNearHeight;

        _topLeftFar = _position + _direction * _zFar - _right * zFarWidth + _up * zFarHeight;
        _topRightFar = _position + _direction * _zFar + _right * zFarWidth + _up * zFarHeight;
        _botLeftFar = _position + _direction * _zFar - _right * zFarWidth - _up * zFarHeight;
        _botRightFar = _position + _direction * _zFar + _right * zFarWidth - _up * zFarHeight;

        //create near plane
        glm::vec3 v1 = _botLeftNear - _botRightNear;
        glm::vec3 v2 = _topLeftNear - _botRightNear;

        glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * _zNear);

        _planeNormals[0] = normal;
        _planePositions[0] = pos;

        //create right plane
        v1 = _topRightNear - _botRightNear;
        v2 = _botRightFar - _botRightNear;

        normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * (_zNear + halfDepth)) + _right * halfWidth;

        _planeNormals[1] = normal;
        _planePositions[1] = pos;

        //create top plane
        v1 = _topLeftNear - _topRightNear;
        v2 = _topRightFar - _topRightNear;

        normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * (_zNear + halfDepth)) + _up * halfHeight;

        _planeNormals[2] = normal;
        _planePositions[2] = pos;

        //create left plane
        v1 = _botLeftFar - _botLeftNear;
        v2 = _topLeftNear - _botLeftNear;

        normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * (_zNear + halfDepth)) - _right * halfWidth;

        _planeNormals[3] = normal;
        _planePositions[3] = pos;

        //create bot plane
        v1 = _botRightFar - _botRightNear;
        v2 = _botLeftNear - _botRightNear;

        normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * (_zNear + halfDepth)) - _up * halfHeight;

        _planeNormals[4] = normal;
        _planePositions[4] = pos;

        //create far plane
        v1 = _botRightFar - _botLeftFar;
        v2 = _topRightFar - _botLeftFar;

        normal = glm::normalize(glm::cross(v1, v2));
        pos = _position + (_direction * _zFar);

        _planeNormals[5] = normal;
        _planePositions[5] = pos;

        _changed = false;
    }

    void frustum::debugRender()
    {
    }

    bool frustum::isInside(glm::vec3 center, float radius)
    {
        glm::vec3 normal;
        glm::vec3 pos;
        float dist;

        for (unsigned int i = 0; i < 6; i++)
        {
            //plane X bounding-sphere intersection
            //plane normal
            normal = _planeNormals[i];
            //object position in relation to plane
            pos = center - _planePositions[i];
            // projection of object position vector into plane normal vector, 
            // a positive value means the object is in the same side of plane's normal
            dist = glm::dot(normal, pos);

            if (dist + radius < 0)
                return false;
        }

        return true;
    }
}