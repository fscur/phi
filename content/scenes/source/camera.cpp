#include "camera.h"
#include "mathUtils.h"
#include "globals.h"

namespace phi
{
    camera::camera(float zNear, float zFar, float aspect, float fov, float focusDistance)  : 
        sceneObject()
    {
        _focus = focusDistance;
        _zoomFactor = 0.3f;
        _direction = glm::vec3(0.0f, 0.0f, 1.0f);

        _frustum = new frustum(glm::vec3(), _direction, _up, zNear, zFar, aspect, fov);

        _orthoProjMatrix = glm::ortho<float>(-5.0, 5.0, -5.0, 5.0, 0.0, 50.0);

        _boundsRadius = 2000.0f;
        _minHeight = 0.1f;
        _maxHeight = _boundsRadius * 0.98f;
        _changed = true;
    }

    camera::~camera(void)
    {
    }

    void camera::setFrustum(frustum* frustum)
    {
        _frustum = frustum;
        _changed = true;
    }

    void camera::setTarget(glm::vec3 target)
    {
        _target = target;
        _direction = _target - _position;
        _focus = glm::length(_direction);
        updateCoordinateSystem();
        _changed = true;
    }

    void camera::setFocus(float focus)
    {
        _focus = focus;
        _target = _direction * focus;
        _changed = true;
    }

    void camera::update()
    {
        _frustum->update();

        //if (getChanged())
        //{
        updateCoordinateSystem();
        _target = _position + (_direction * _focus);
        _viewMatrix = glm::lookAt(_position, _target, _up);

        _frustum->setPosition(_position);
        _frustum->setDirection(_direction);
        _frustum->setUp(_up);

        _changed = false;
        //}
    }

    void camera::updateCoordinateSystem()
    {
        _direction = glm::normalize(_direction);

        _right = glm::cross(_direction, glm::vec3(0.0f, 1.0f, 0.0f));
        _up = glm::cross(_right, _direction);

        _right = glm::normalize(_right);
        _up = glm::normalize(_up);
        _direction = glm::normalize(_direction);
    }

    void camera::setPositionWithinBoundsRadius(glm::vec3 position)
    {
        /*glm::vec3 p = glm::normalize(position);

        float dot = glm::dot(p, glm::vec3(0.0f, 1.0f, 0.0f));

        if (dot > 0.97)
        return;

        float radius = _boundsRadius;
        float distToOrigin = glm::length(position);
        float diff = distToOrigin - radius;

        if (diff > 0)
        {
        glm::vec3 dir = glm::normalize(-position);
        position += dir * diff;
        }
        else
        radius = distToOrigin;

        if (position.y < _minHeight)
        {
        position = glm::normalize(glm::vec3(position.x, _minHeight, position.z)) * distToOrigin;
        }*/

        _position = position;
    }

    void camera::orbit(glm::vec3 origin, glm::vec3 axis, float angle)
    {  
        glm::vec3 position = mathUtils::rotateAboutAxis(_position, origin, axis, angle);

        //if (position.y >= _minHeight)
        setPositionWithinBoundsRadius(position);

        if (_position == position)
            _target = mathUtils::rotateAboutAxis(_target, origin, axis, angle);

        _direction = _target - _position;
        _changed = true;
    }

    void camera::moveTo(glm::vec3 position)
    {
        setPositionWithinBoundsRadius(position);

        _changed = true;
    }

    void camera::zoomIn(glm::vec3 targetPos)
    {
        float dist = mathUtils::distance(targetPos, _position);
        glm::vec3 direction = glm::normalize(targetPos - _position);

        float factor = _zoomFactor;

        if (dist < 1.0)
            factor = 1 - (1/(pow(2.0f, dist)));

        dist *= factor;

        if (dist < _frustum->getZNear() + 0.25f)
            return;

        glm::vec3 offset = direction * dist;

        _focus = glm::max(_focus - dist, _frustum->getZNear());

        glm::vec3 position = _position + offset;

        setPositionWithinBoundsRadius(position);

        //_direction = _target - _position;

        _changed = true;
    }

    void camera::zoomOut(glm::vec3 targetPos)
    {
        float dist = mathUtils::distance(targetPos, _position);
        glm::vec3 direction = glm::normalize(targetPos - _position);

        dist *= -_zoomFactor / (1 -_zoomFactor);

        glm::vec3 offset = direction * dist;
        _focus -= dist;

        glm::vec3 position = _position + offset;

        setPositionWithinBoundsRadius(position);

        //_direction = _target - _position;

        _changed = true;
    }

    void camera::debugRender()
    {
        _frustum->debugRender();

        //float tanHalfFov = tan(_fov/2);
        //float zNearWidth = tanHalfFov * _znear;
        //float zNearHeight = zNearWidth * _fov;
        //float zFarWidth = tanHalfFov * _zfar;
        //float zFarHeight = zFarWidth * _fov;

        //glm::vec3 pos = _position;

        //glm::vec3 topLeftNear = pos + (_direction * _znear) - _right * zNearWidth + _up * zNearHeight;
        //glm::vec3 topRightNear = pos + (_direction * _znear) + _right * zNearWidth + _up * zNearHeight;
        //glm::vec3 botLeftNear = pos + (_direction * _znear) - _right * zNearWidth - _up * zNearHeight;
        //glm::vec3 botRightNear = pos + (_direction * _znear) + _right * zNearWidth - _up * zNearHeight;

        //glm::vec3 topLeftFar = pos + (_direction * _zfar) - _right * zFarWidth + _up * zFarHeight;
        //glm::vec3 topRightFar = pos + (_direction * _zfar) + _right * zFarWidth + _up * zFarHeight;
        //glm::vec3 botLeftFar = pos + (_direction * _zfar) - _right * zFarWidth - _up * zFarHeight;
        //glm::vec3 botRightFar = pos + (_direction * _zfar) + _right * zFarWidth - _up * zFarHeight;

        //glPushMatrix();
        //// front quad

        //glColor3f(1.0f, 0.0f, 0.0f);

        //glVertex3f(botLeftNear.x, botLeftNear.y, botLeftNear.z); glVertex3f(botRightNear.x, botRightNear.y, botRightNear.z);
        //glVertex3f(botRightNear.x, botRightNear.y, botRightNear.z); glVertex3f(topRightNear.x, topRightNear.y, topRightNear.z);
        //glVertex3f(topRightNear.x, topRightNear.y, topRightNear.z); glVertex3f(topLeftNear.x, topLeftNear.y, topLeftNear.z);
        //glVertex3f(topLeftNear.x, topLeftNear.y, topLeftNear.z); glVertex3f(botLeftNear.x, botLeftNear.y, botLeftNear.z);

        //glVertex3f(botLeftFar.x, botLeftFar.y, botLeftFar.z); glVertex3f(botRightFar.x, botRightFar.y, botRightFar.z);
        //glVertex3f(botRightFar.x, botRightFar.y, botRightFar.z); glVertex3f(topRightFar.x, topRightFar.y, topRightFar.z);
        //glVertex3f(topRightFar.x, topRightFar.y, topRightFar.z); glVertex3f(topLeftFar.x, topLeftFar.y, topLeftFar.z);
        //glVertex3f(topLeftFar.x, topLeftFar.y, topLeftFar.z); glVertex3f(botLeftFar.x, botLeftFar.y, botLeftFar.z);

        //glColor3f(1.0f, 1.0f, 0.0f);

        //glVertex3f(pos.x, pos.y, pos.z); glVertex3f(topLeftFar.x, topLeftFar.y, topLeftFar.z);
        //glVertex3f(pos.x, pos.y, pos.z); glVertex3f(topRightFar.x, topRightFar.y, topRightFar.z);
        //glVertex3f(pos.x, pos.y, pos.z); glVertex3f(botLeftFar.x, botLeftFar.y, botLeftFar.z);
        //glVertex3f(pos.x, pos.y, pos.z); glVertex3f(botRightFar.x, botRightFar.y, botRightFar.z);

        //glColor3f(1.0f, 0.0f, 0.0f);
        //glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
        //glColor3f(0.0f, 1.0f, 0.0f);
        //glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
        //glColor3f(0.0f, 0.0f, 1.0f);
        //glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
        //glPopMatrix();
    }

    ray camera::castRay(glm::vec2 screenCoords, size<unsigned int> screenSize)
    {
        float w = (float)screenSize.width;
        float h = (float)screenSize.height;
        float x = (2 * screenCoords.x)/w - 1.0f; 
        float y = 1.0f - (2 * screenCoords.y)/h;

        glm::mat4 invPersp = glm::inverse(_frustum->getPerspMatrix());
        glm::mat4 invView = glm::inverse(glm::lookAt(_position, _target, _up));

        glm::vec4 ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
        glm::vec4 ray_eye = invPersp * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
        glm::vec3 ray_world = glm::vec3(invView * ray_eye);
        ray_world = glm::normalize(ray_world);

        return ray(_position, ray_world);
    }
}