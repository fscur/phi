#include "defaultCameraController.h"

#include <core/inputKey.h>
#include <core/globals.h>
#include <rendering/renderingSystem.h>
#include <rendering/camera.h>

defaultCameraController::defaultCameraController(phi::camera* camera) : 
    cameraController(camera)
{
    _rotating = false;
    _panning = false;
    _shortcuts.add("Pan", phi::inputKey(PHI_MOUSE_MIDDLE, PHI_NONE));
    _shortcuts.add("Rotate", phi::inputKey(PHI_MOUSE_RIGHT, PHI_NONE));
    _shortcuts.add("ZoomIn", phi::inputKey(PHI_MOUSE_WHEEL_UP, PHI_NONE));
    _shortcuts.add("ZoomOut", phi::inputKey(PHI_MOUSE_WHEEL_DOWN, PHI_NONE));
}

void defaultCameraController::executeInput(phi::inputKey key, phi::vec2 mousePos)
{
    auto cmd = _shortcuts.getCommand(key);
    if (cmd == "Pan")
        initPan(mousePos);
    else if (cmd == "Rotate")
        initRotate(mousePos);
    else if (cmd == "ZoomIn")
        zoom(mousePos, true);
    else if (cmd == "ZoomOut")
        zoom(mousePos, false);
}

void defaultCameraController::initPan(phi::vec2 mousePos)
{
    _zBufferValue = phi::renderingSystem::defaultFrameBuffer->getZBufferValue(mousePos);
    phi::mat4 proj = _camera->getProjectionMatrix();

    if (_zBufferValue == 1.0f)
        _eyeZ = 20.0f;
    else
        _eyeZ = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);

    _cameraPos = _camera->getPosition();
    _cameraRight = _camera->getRight();
    _cameraUp = _camera->getUp();
    _startPos = mousePos;
    _panning = true;
}

void defaultCameraController::initRotate(phi::vec2 mousePos)
{
    _zBufferValue = phi::renderingSystem::defaultFrameBuffer->getZBufferValue(mousePos);

    phi::mat4 proj = _camera->getProjectionMatrix();

    _lastMousePos = mousePos;

    float z = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);
    if (_zBufferValue == 1.0f)
        //_targetPos = phi::scenesManager::get()->getScene()->getAabb()->getCenter();
        _targetPos = glm::vec3();
    else
    {
        phi::frustum* frustum = _camera->getFrustum();

        auto zNear = frustum->getZNear();
        auto iez = 1.0f / zNear;
        auto zFar = frustum->getZFar();
        auto aspect = frustum->getAspect();
        auto fov = frustum->getFov();

        auto tg = tan(fov * 0.5f) * zNear;

        auto h = _camera->getResolution().h;
        auto w = _camera->getResolution().w;

        auto hh = h * 0.5f;
        auto hw = w * 0.5f;

        auto ys0 = mousePos.y - hh;
        auto yp0 = ys0 / hh;
        auto ym0 = -(yp0 * tg);

        auto xs0 = mousePos.x - hw;
        auto xp0 = xs0 / hw;
        auto xm0 = xp0 * tg * aspect;

        auto x = (xm0 / zNear) * (z);
        auto y = (ym0 / zNear) * (z);

        auto camPos = _camera->getPosition();
        auto camDir = _camera->getDirection();
        auto camRight = _camera->getRight();
        auto camUp = _camera->getUp();

        _targetPos = camPos + camDir * z + -camRight * x + camUp * y;
    }

    _eyeZ = z;
    _rotating = true;
}

void defaultCameraController::pan(phi::vec2 mousePos)
{
    auto frustum = _camera->getFrustum();

    auto zNear = frustum->getZNear();
    auto iez = 1.0f / zNear;
    auto zFar = frustum->getZFar();
    auto aspect = frustum->getAspect();
    auto fov = frustum->getFov();

    auto tg = tan(fov * 0.5f) * zNear;

    auto h = _camera->getResolution().h;
    auto w = _camera->getResolution().w;

    auto hh = h * 0.5f;
    auto hw = w * 0.5f;

    auto ys0 = _startPos.y - hh;
    auto yp0 = ys0/hh;
    auto ym0 = -(yp0 * tg);

    auto xs0 = _startPos.x - hw;
    auto xp0 = xs0/hw;
    auto xm0 = xp0 * tg * aspect;

    auto ys1 = mousePos.y - hh;
    auto yp1 = ys1/hh;
    auto ym1 = -(yp1 * tg);

    auto xs1 = mousePos.x - hw;
    auto xp1 = xs1/hw;
    auto xm1 = xp1 * tg * aspect;

    auto eyeZ = _eyeZ;

    auto xDiff = xm1 - xm0;
    auto yDiff = ym1 - ym0;

    auto x = xDiff * (eyeZ/zNear);
    auto y = yDiff * (eyeZ/zNear);

    phi::vec3 pos = _cameraPos - (phi::vec3(-_cameraRight * x) + phi::vec3(_cameraUp * y)); 

    _camera->moveTo(pos);
}

void defaultCameraController::rotate(phi::vec2 mousePos)
{
    auto camera = *_camera;
    auto frustum = _camera->getFrustum();

    auto zNear = frustum->getZNear();
    auto iez = 1.0f / zNear;
    auto zFar = frustum->getZFar();
    auto aspect = frustum->getAspect();
    auto fov = frustum->getFov();

    auto tg = tan(fov * 0.5f) * zNear;

    auto h = _camera->getResolution().h;
    auto w = _camera->getResolution().w;

    auto dx = _lastMousePos.x - mousePos.x;
    auto dy = _lastMousePos.y - mousePos.y;

    auto x = (dx/w) * 3 * phi::PI;
    auto y = (dy/h) * 3 * phi::PI;

    _camera->orbit(_targetPos, phi::vec3(0.0f, 1.0f, 0.0f), -_camera->getRight(), x, y);
    _camera->update();

    _lastMousePos = mousePos;
}

void defaultCameraController::zoom(phi::vec2 mousePos, bool in)
{
    _zBufferValue = phi::renderingSystem::defaultFrameBuffer->getZBufferValue(mousePos);

    auto camera = *_camera;
    phi::mat4 proj = _camera->getProjectionMatrix();

    float z;
    if (_zBufferValue == 1.0f)
        z = 10.0f;
    else
        z = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);

    phi::frustum* frustum = _camera->getFrustum();

    auto zNear = frustum->getZNear();
    auto iez = 1.0f / zNear;
    auto zFar = frustum->getZFar();
    auto aspect = frustum->getAspect();
    auto fov = frustum->getFov();

    auto tg = tan(fov * 0.5f) * zNear;

    auto h = _camera->getResolution().h;
    auto w = _camera->getResolution().w;

    auto hh = h * 0.5f;
    auto hw = w * 0.5f;

    auto ys0 = mousePos.y - hh;
    auto yp0 = ys0/hh;
    auto ym0 = -(yp0 * tg);

    auto xs0 = mousePos.x - hw;
    auto xp0 = xs0/hw;
    auto xm0 = xp0 * tg * aspect;

    auto x = (xm0/zNear) * z;
    auto y = (ym0/zNear) * z;

    phi::vec3 camPos = _camera->getPosition();
    phi::vec3 camDir = _camera->getDirection();
    phi::vec3 camRight = _camera->getRight();
    phi::vec3 camUp = _camera->getUp();

    phi::vec3 targetPos = camPos + camDir * z + -camRight * x + camUp * y;

    if (in)
        _camera->zoomIn(targetPos);
    else
        _camera->zoomOut(targetPos);
}

bool defaultCameraController::onMouseDown(phi::mouseEventArgs* e, phi::inputKey key)
{
    executeInput(key, phi::vec2(e->x, e->y));
    return false;
}

bool defaultCameraController::onMouseMove(phi::mouseEventArgs* e)
{
    phi::vec2 mousePos = phi::vec2(e->x, e->y);

    if (_rotating)
        rotate(mousePos);
    if (_panning)
        pan(mousePos);

    return false;
}

bool defaultCameraController::onMouseUp(phi::mouseEventArgs* e, phi::inputKey key)
{
    _panning = _rotating = false;
    return false;
}

bool defaultCameraController::onMouseWheel(phi::mouseEventArgs* e, phi::inputKey key)
{
    executeInput(key, phi::vec2(e->x, e->y));
    return false;
}

void defaultCameraController::update()
{
}