#include <precompiled.h>
#include "defaultCameraController.h"

#include <rendering/camera.h>

namespace demon
{
    defaultCameraController::defaultCameraController(phi::scene* scene) :
        _scene(scene),
        cameraController(scene->camera)
    {
        _rotating = false;
        _panning = false;
    }

    void defaultCameraController::initPan(float mouseX, float mouseY)
    {
        _zBufferValue = _scene->getZBufferValue(mouseX, mouseY);
        phi::mat4 proj = _camera->getProjectionMatrix();

        if (_zBufferValue == 1.0f)
            _eyeZ = 20.0f;
        else
            _eyeZ = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);

        auto cameraTransform = _camera->getTransform();
        _cameraPos = cameraTransform->getPosition();
        _cameraRight = cameraTransform->getRight();
        _cameraUp = cameraTransform->getUp();
        _startPosX = mouseX;
        _startPosY = mouseY;
        _panning = true;
    }

    void defaultCameraController::initRotate(float mouseX, float mouseY)
    {
        _zBufferValue = _scene->getZBufferValue(mouseX, mouseY);

        phi::mat4 proj = _camera->getProjectionMatrix();

        _lastMousePosX = mouseX;
        _lastMousePosY = mouseY;

        float z = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);
        if (_zBufferValue == 1.0f)
            //_targetPos = phi::scenesManager::get()->getScene()->getAabb()->getCenter();
            _targetPos = glm::vec3();
        else
        {
            auto zNear = _camera->getZNear();
            auto iez = 1.0f / zNear;
            auto zFar = _camera->getZFar();
            auto aspect = _camera->getAspect();
            auto fov = _camera->getFov();

            auto tg = tan(fov * 0.5f) * zNear;

            auto w = _camera->getResolution().x;
            auto h = _camera->getResolution().y;

            auto hh = h * 0.5f;
            auto hw = w * 0.5f;

            auto ys0 = mouseY - hh;
            auto yp0 = ys0 / hh;
            auto ym0 = -(yp0 * tg);

            auto xs0 = mouseX - hw;
            auto xp0 = xs0 / hw;
            auto xm0 = xp0 * tg * aspect;

            auto x = (xm0 / zNear) * (z);
            auto y = (ym0 / zNear) * (z);

            auto transform = _camera->getTransform();
            auto camPos = transform->getPosition();
            auto camDir = transform->getDirection();
            auto camRight = transform->getRight();
            auto camUp = transform->getUp();

            _targetPos = camPos + camDir * z + -camRight * (float)x + camUp * (float)y;
        }

        _eyeZ = z;
        _rotating = true;
    }

    void defaultCameraController::pan(float mouseX, float mouseY)
    {
        auto zNear = _camera->getZNear();
        auto iez = 1.0f / zNear;
        auto zFar = _camera->getZFar();
        auto aspect = _camera->getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto w = _camera->getResolution().x;
        auto h = _camera->getResolution().y;

        auto hh = h * 0.5f;
        auto hw = w * 0.5f;

        auto ys0 = (float)_startPosY - hh;
        auto yp0 = ys0 / hh;
        auto ym0 = -(yp0 * tg);

        auto xs0 = (float)_startPosX - hw;
        auto xp0 = xs0 / hw;
        auto xm0 = xp0 * tg * aspect;

        auto ys1 = (float)mouseY - hh;
        auto yp1 = ys1 / hh;
        auto ym1 = -(yp1 * tg);

        auto xs1 = (float)mouseX - hw;
        auto xp1 = xs1 / hw;
        auto xm1 = xp1 * tg * aspect;

        auto eyeZ = _eyeZ;

        auto xDiff = xm1 - xm0;
        auto yDiff = ym1 - ym0;

        float x = xDiff * (eyeZ / zNear);
        float y = yDiff * (eyeZ / zNear);

        phi::vec3 pos = _cameraPos - (phi::vec3(-_cameraRight * x) + phi::vec3(_cameraUp * y));

        _camera->moveTo(pos);
    }

    void defaultCameraController::rotate(float mouseX, float mouseY)
    {
        auto zNear = _camera->getZNear();
        auto iez = 1.0f / zNear;
        auto zFar = _camera->getZFar();
        auto aspect = _camera->getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto w = _camera->getResolution().x;
        auto h = _camera->getResolution().y;

        auto dx = _lastMousePosX - mouseX;
        auto dy = _lastMousePosY - mouseY;

        auto x = (dx / w) * 3 * phi::PI;
        auto y = (dy / h) * 3 * phi::PI;

        _camera->orbit(_targetPos, phi::vec3(0.0f, 1.0f, 0.0f), -_camera->getTransform()->getRight(), x, y);

        _lastMousePosX = mouseX;
        _lastMousePosY = mouseY;
    }

    void defaultCameraController::zoom(float mouseX, float mouseY, bool in)
    {
        _zBufferValue = _scene->getZBufferValue(mouseX, mouseY);

        auto camera = *_camera;
        phi::mat4 proj = _camera->getProjectionMatrix();

        float z;
        if (_zBufferValue == 1.0f)
            z = 10.0f;
        else
            z = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);

        auto zNear = _camera->getZNear();
        auto iez = 1.0f / zNear;
        auto zFar = _camera->getZFar();
        auto aspect = _camera->getAspect();
        auto fov = _camera->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto w = _camera->getResolution().x;
        auto h = _camera->getResolution().y;

        auto hh = h * 0.5f;
        auto hw = w * 0.5f;

        auto ys0 = mouseY - hh;
        auto yp0 = ys0 / hh;
        auto ym0 = -(yp0 * tg);

        auto xs0 = mouseX - hw;
        auto xp0 = xs0 / hw;
        auto xm0 = xp0 * tg * aspect;

        auto x = (xm0 / zNear) * z;
        auto y = (ym0 / zNear) * z;

        auto transform = _camera->getTransform();
        phi::vec3 camPos = transform->getPosition();
        phi::vec3 camDir = transform->getDirection();
        phi::vec3 camRight = transform->getRight();
        phi::vec3 camUp = transform->getUp();

        phi::vec3 targetPos = camPos + camDir * (float)z + -camRight * (float)x + camUp * (float)y;

        if (in)
            _camera->zoomIn(targetPos);
        else
            _camera->zoomOut(targetPos);
    }

    void defaultCameraController::onMouseDown(phi::mouseEventArgs* e)
    {
        if (e->rightButtonPressed)
            initRotate(e->x, e->y);
        else if (e->middleButtonPressed)
            initPan(e->x, e->y);
    }

    void defaultCameraController::onMouseMove(phi::mouseEventArgs* e)
    {
        if (_rotating)
            rotate(e->x, e->y);
        if (_panning)
            pan(e->x, e->y);
    }

    void defaultCameraController::onMouseUp(phi::mouseEventArgs* e)
    {
        _panning = _rotating = false;
    }

    void defaultCameraController::onMouseWheel(phi::mouseEventArgs* e)
    {
        zoom(e->x, e->y, e->wheelDelta > 0.0f);
    }

    void defaultCameraController::update()
    {
    }
}