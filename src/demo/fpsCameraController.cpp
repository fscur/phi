#include "phi/demo/fpsCameraController.h"

#include "phi/core/globals.h"
#include "phi/core/mathUtils.h"

#include "phi/scenes/camera.h"
#include "phi/scenes/scenesManager.h"

#include <SDL\SDL.h>
#include <glm/gtx/vector_angle.hpp>

fpsCameraController::fpsCameraController(phi::size<unsigned int> viewportSize) :
    cameraController(viewportSize)
{
    _walkForward = new phi::inputKey(PHI_SCANCODE_W, 0);
    _walkBackward = new phi::inputKey(PHI_SCANCODE_S, 0);
    _walkLeft = new phi::inputKey(PHI_SCANCODE_A, 0);
    _walkRight = new phi::inputKey(PHI_SCANCODE_D, 0);

    int x, y;
    SDL_GetMouseState(&x, &y);
    _lastMousePos = glm::vec2(x, y);
}

bool fpsCameraController::onMouseDown(phi::mouseEventArgs* e, phi::inputKey key)
{
    return false;
}

bool fpsCameraController::onMouseMove(phi::mouseEventArgs* e)
{
    auto mousePos = glm::vec2(e->x, e->y);
    phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
    auto diff = mousePos - _lastMousePos;
    //camera->yaw(diff.x * 0.001f);
    camera->rotate(-diff.x * 0.001f, camera->getUp());
    camera->rotate(diff.y * 0.001f, camera->getRight());
    //camera->pitch(diff.y * 0.001f);
    //camera->update();

    //auto dir = camera->getDirection();
    //auto right = glm::normalize(glm::cross(dir, glm::vec3(0.0f, -1.0f, 0.0f)));

    //auto q1 = phi::mathUtils::rotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), dir);
    //auto angle = glm::orientedAngle(q1 * glm::vec3(1.0f, 0.0f, 0.0f), right, dir);
    //auto q2 = glm::angleAxis(angle, dir);

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
    glm::vec3 moveDir;
    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
    phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();
    if (currentKeyStates[_walkForward->key])
        moveDir += camera->getDirection();
    if (currentKeyStates[_walkBackward->key])
        moveDir += -camera->getDirection();
    if (currentKeyStates[_walkLeft->key])
        moveDir += camera->getRight();
    if (currentKeyStates[_walkRight->key])
        moveDir += -camera->getRight();

    if (moveDir != glm::vec3())
    {
        camera->setLocalPosition(camera->getLocalPosition() + glm::normalize(moveDir) * 0.2f);
        return true;
    }

    return false;
}

void fpsCameraController::update()
{
}