#include <phi\demo\fpsCameraController.h>
#include <phi\core\globals.h>
#include <phi\core\mathUtils.h>
#include <SDL\SDL.h>
#include <glm\gtx\vector_angle.hpp>

fpsCameraController::fpsCameraController(phi::camera* camera) :
    cameraController(camera)
{
    _walkForward = new phi::inputKey(PHI_SCANCODE_W, 0);
    _walkBackward = new phi::inputKey(PHI_SCANCODE_S, 0);
    _walkLeft = new phi::inputKey(PHI_SCANCODE_A, 0);
    _walkRight = new phi::inputKey(PHI_SCANCODE_D, 0);

    int x, y;
    SDL_GetMouseState(&x, &y);
    _lastMousePos = phi::vec2(x, y);
}

bool fpsCameraController::onMouseDown(phi::mouseEventArgs* e, phi::inputKey key)
{
    return false;
}

bool fpsCameraController::onMouseMove(phi::mouseEventArgs* e)
{
    auto mousePos = phi::vec2(e->x, e->y);
    auto diff = mousePos - _lastMousePos;
    //camera->yaw(diff.x * 0.001f);
    _camera->rotate(-diff.x * 0.001f, _camera->getUp());
    _camera->rotate(diff.y * 0.001f, _camera->getRight());
    //camera->pitch(diff.y * 0.001f);
    //camera->update();

    //auto dir = camera->getDirection();
    //auto right = normalize(cross(dir, phi::vec3(0.0f, -1.0f, 0.0f)));

    //auto q1 = phi::mathUtils::rotationBetweenVectors(phi::vec3(0.0f, 0.0f, 1.0f), dir);
    //auto angle = orientedAngle(q1 * phi::vec3(1.0f, 0.0f, 0.0f), right, dir);
    //auto q2 = angleAxis(angle, dir);

    //camera->setOrientation(q2 * q1);

    _lastMousePos = mousePos;
    return true;
}

bool fpsCameraController::onMouseUp(phi::mouseEventArgs* e, phi::inputKey key)
{
    return false;
}

bool fpsCameraController::onMouseWheel(phi::mouseEventArgs* e, phi::inputKey key)
{
    return false;
}

bool fpsCameraController::onKeyDown(phi::keyboardEventArgs* e, phi::inputKey key)
{
    phi::vec3 moveDir;
    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);

    if (currentKeyStates[_walkForward->key])
        moveDir += _camera->getDirection();
    if (currentKeyStates[_walkBackward->key])
        moveDir += -_camera->getDirection();
    if (currentKeyStates[_walkLeft->key])
        moveDir += _camera->getRight();
    if (currentKeyStates[_walkRight->key])
        moveDir += -_camera->getRight();

    if (moveDir != phi::vec3())
    {
        _camera->setLocalPosition(_camera->getLocalPosition() + normalize(moveDir) * 0.2f);
        return true;
    }

    return false;
}

void fpsCameraController::update()
{
}