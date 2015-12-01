#include "phi/demo/defaultCameraController.h"

#include "phi/core/inputKey.h"
#include "phi/core/globals.h"

#include "phi/rendering/renderingSystem.h"

#include "phi/scenes/camera.h"
#include "phi/scenes/scenesManager.h"

defaultCameraController::defaultCameraController(phi::size<unsigned int> viewportSize) :
    cameraController(viewportSize)
{
    _rotating = false;
    _panning = false;
    _shortcuts.add("Pan", phi::inputKey(PHI_MOUSE_MIDDLE, PHI_NONE));
    _shortcuts.add("Rotate", phi::inputKey(PHI_MOUSE_RIGHT, PHI_NONE));
    _shortcuts.add("ZoomIn", phi::inputKey(PHI_MOUSE_WHEEL_UP, PHI_NONE));
    _shortcuts.add("ZoomOut", phi::inputKey(PHI_MOUSE_WHEEL_DOWN, PHI_NONE));
}

void defaultCameraController::executeInput(phi::inputKey key, glm::vec2 mousePos)
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

void defaultCameraController::initPan(glm::vec2 mousePos)
{
    _zBufferValue = phi::renderingSystem::defaultFrameBuffer->getZBufferValue(mousePos);
    phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
    glm::mat4 proj = camera->getPerspProjMatrix();

    if (_zBufferValue == 1.0f)
        _eyeZ = 20.0f;
    else
        _eyeZ = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);

    _cameraPos = camera->getPosition();
    _cameraRight = camera->getRight();
    _cameraUp = camera->getUp();
    _startPos = mousePos;
    _panning = true;
}

void defaultCameraController::initRotate(glm::vec2 mousePos)
{
    _zBufferValue = phi::renderingSystem::defaultFrameBuffer->getZBufferValue(mousePos);

    phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
    glm::mat4 proj = camera->getPerspProjMatrix();

    _lastMousePos = mousePos;

    float z = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);
    if (_zBufferValue == 1.0f)
        _targetPos = phi::scenesManager::get()->getScene()->getAabb()->getCenter();
    else
    {
        phi::frustum* frustum = camera->getFrustum();

        auto zNear = frustum->getZNear();
        auto iez = 1.0f / zNear;
        auto zFar = frustum->getZFar();
        auto aspect = frustum->getAspect();
        auto fov = frustum->getFov();

        auto tg = glm::tan(fov * 0.5f) * zNear;

        auto h = _viewportSize.height;
        auto w = _viewportSize.width;

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

        auto camPos = camera->getPosition();
        auto camDir = camera->getDirection();
        auto camRight = camera->getRight();
        auto camUp = camera->getUp();

        _targetPos = camPos + camDir * z + -camRight * x + camUp * y;
    }

    _eyeZ = z;
    _rotating = true;
}

void defaultCameraController::pan(glm::vec2 mousePos)
{
    auto camera = phi::scenesManager::get()->getScene()->getActiveCamera();
    auto frustum = camera->getFrustum();

    auto zNear = frustum->getZNear();
    auto iez = 1.0f / zNear;
    auto zFar = frustum->getZFar();
    auto aspect = frustum->getAspect();
    auto fov = frustum->getFov();

    auto tg = glm::tan(fov * 0.5f) * zNear;

    auto h = _viewportSize.height;
    auto w = _viewportSize.width;

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

    glm::vec3 pos = _cameraPos - (glm::vec3(-_cameraRight * x) + glm::vec3(_cameraUp * y)); 

    camera->moveTo(pos);
}

void defaultCameraController::rotate(glm::vec2 mousePos)
{
    auto camera = phi::scenesManager::get()->getScene()->getActiveCamera();
    auto frustum = camera->getFrustum();

    auto zNear = frustum->getZNear();
    auto iez = 1.0f / zNear;
    auto zFar = frustum->getZFar();
    auto aspect = frustum->getAspect();
    auto fov = frustum->getFov();

    auto tg = glm::tan(fov * 0.5f) * zNear;

    auto h = _viewportSize.height;
    auto w = _viewportSize.width;

    auto dx = _lastMousePos.x - mousePos.x;
    auto dy = _lastMousePos.y - mousePos.y;

    auto x = (dx/w) * 3 * phi::PI;
    auto y = (dy/h) * 3 * phi::PI;

    camera->orbit(_targetPos, glm::vec3(0.0f, 1.0f, 0.0f), -camera->getRight(), x, y);
    camera->update();

    _lastMousePos = mousePos;
}

void defaultCameraController::zoom(glm::vec2 mousePos, bool in)
{
    _zBufferValue = phi::renderingSystem::defaultFrameBuffer->getZBufferValue(mousePos);

    phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
    glm::mat4 proj = camera->getPerspProjMatrix();

    float z;
    if (_zBufferValue == 1.0f)
        z = 10.0f;
    else
        z = -proj[3].z / (_zBufferValue * -2.0f + 1.0f - proj[2].z);

    phi::frustum* frustum = camera->getFrustum();

    auto zNear = frustum->getZNear();
    auto iez = 1.0f / zNear;
    auto zFar = frustum->getZFar();
    auto aspect = frustum->getAspect();
    auto fov = frustum->getFov();

    auto tg = glm::tan(fov * 0.5f) * zNear;

    auto h = _viewportSize.height;
    auto w = _viewportSize.width;

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

    glm::vec3 camPos = camera->getPosition();
    glm::vec3 camDir = camera->getDirection();
    glm::vec3 camRight = camera->getRight();
    glm::vec3 camUp = camera->getUp();

    glm::vec3 targetPos = camPos + camDir * z + -camRight * x + camUp * y;

    if (in)
        camera->zoomIn(targetPos);
    else
        camera->zoomOut(targetPos);
}

bool defaultCameraController::onMouseDown(phi::mouseEventArgs* e, phi::inputKey key)
{
    executeInput(key, glm::vec2(e->x, e->y));
    return false;
}

bool defaultCameraController::onMouseMove(phi::mouseEventArgs* e)
{
    glm::vec2 mousePos = glm::vec2(e->x, e->y);

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
    executeInput(key, glm::vec2(e->x, e->y));
    return false;
}

void defaultCameraController::update()
{
}