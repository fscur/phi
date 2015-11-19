#include "defaultCameraController.h"
#include "inputKey.h"
#include "renderingSystem.h"
#include "camera.h"
#include "scenesManager.h"
#include "globals.h"

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
        _eyeZ = -proj[3].z / (_zBufferValue * -2.0 + 1.0 - proj[2].z);

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

    float z = -proj[3].z / (_zBufferValue * -2.0 + 1.0 - proj[2].z);
    if (_zBufferValue == 1.0f)
        _targetPos = phi::scenesManager::get()->getScene()->getAabb()->getCenter();
    else
    {
        phi::frustum* frustum = camera->getFrustum();

        float zNear = frustum->getZNear();
        float iez = 1.0f / zNear;
        float zFar = frustum->getZFar();
        float aspect = frustum->getAspect();
        float fov = frustum->getFov();

        float tg = glm::tan(fov * 0.5f) * zNear;

        float h = _viewportSize.height;
        float w = _viewportSize.width;

        float hh = h * 0.5f;
        float hw = w * 0.5f;

        float ys0 = mousePos.y - hh;
        float yp0 = ys0 / hh;
        float ym0 = -(yp0 * tg);

        float xs0 = mousePos.x - hw;
        float xp0 = xs0 / hw;
        float xm0 = xp0 * tg * aspect;

        float x = (xm0 / zNear) * (z);
        float y = (ym0 / zNear) * (z);

        glm::vec3 camPos = camera->getPosition();
        glm::vec3 camDir = camera->getDirection();
        glm::vec3 camRight = camera->getRight();
        glm::vec3 camUp = camera->getUp();

        _targetPos = camPos + camDir * z + -camRight * x + camUp * y;
    }

    _eyeZ = z;
    _rotating = true;
}

void defaultCameraController::pan(glm::vec2 mousePos)
{
    phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
    phi::frustum* frustum = camera->getFrustum();

    float zNear = frustum->getZNear();
    float iez = 1.0f / zNear;
    float zFar = frustum->getZFar();
    float aspect = frustum->getAspect();
    float fov = frustum->getFov();

    float tg = glm::tan(fov * 0.5f) * zNear;

    float h = _viewportSize.height;
    float w = _viewportSize.width;

    float hh = h * 0.5f;
    float hw = w * 0.5f;

    float ys0 = _startPos.y - hh;
    float yp0 = ys0/hh;
    float ym0 = -(yp0 * tg);

    float xs0 = _startPos.x - hw;
    float xp0 = xs0/hw;
    float xm0 = xp0 * tg * aspect;

    float ys1 = mousePos.y - hh;
    float yp1 = ys1/hh;
    float ym1 = -(yp1 * tg);

    float xs1 = mousePos.x - hw;
    float xp1 = xs1/hw;
    float xm1 = xp1 * tg * aspect;

    float eyeZ = _eyeZ;

    float xDiff = xm1 - xm0;
    float yDiff = ym1 - ym0;

    float x = xDiff * (eyeZ/zNear);
    float y = yDiff * (eyeZ/zNear);

    glm::vec3 pos = _cameraPos - (glm::vec3(-_cameraRight * x) + glm::vec3(_cameraUp * y)); 

    camera->moveTo(pos);
}

void defaultCameraController::rotate(glm::vec2 mousePos)
{
    phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
    phi::frustum* frustum = camera->getFrustum();

    float zNear = frustum->getZNear();
    float iez = 1.0f / zNear;
    float zFar = frustum->getZFar();
    float aspect = frustum->getAspect();
    float fov = frustum->getFov();

    float tg = glm::tan(fov * 0.5f) * zNear;

    float h = _viewportSize.height;
    float w = _viewportSize.width;

    float dx = _lastMousePos.x - mousePos.x;
    float dy = _lastMousePos.y - mousePos.y;

    float x = (dx/w) * 3 * phi::PI;
    float y = (dy/h) * 3 * phi::PI;

    camera->orbit(_targetPos, glm::vec3(0.0, 1.0, 0.0), -camera->getRight(), x, y);
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
        z = -proj[3].z / (_zBufferValue * -2.0 + 1.0 - proj[2].z);

    phi::frustum* frustum = camera->getFrustum();

    float zNear = frustum->getZNear();
    float iez = 1.0f / zNear;
    float zFar = frustum->getZFar();
    float aspect = frustum->getAspect();
    float fov = frustum->getFov();

    float tg = glm::tan(fov * 0.5f) * zNear;

    float h = _viewportSize.height;
    float w = _viewportSize.width;

    float hh = h * 0.5f;
    float hw = w * 0.5f;

    float ys0 = mousePos.y - hh;
    float yp0 = ys0/hh;
    float ym0 = -(yp0 * tg);

    float xs0 = mousePos.x - hw;
    float xp0 = xs0/hw;
    float xm0 = xp0 * tg * aspect;

    float x = (xm0/zNear) * z;
    float y = (ym0/zNear) * z;

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