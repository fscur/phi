#include "addSceneObjectCommand.h"
#include "scenesManager.h"

addSceneObjectCommand::addSceneObjectCommand(phi::sceneObject* object, glm::vec2 screenPos)
    : _object(object), _screenPos(screenPos), command()
{
    _initialized = false;
}

void addSceneObjectCommand::init()
{
    if (_initialized)
        return;

    _bufferRequest = new phi::bufferRequest((unsigned int)_screenPos.x, (unsigned int)_screenPos.y, (unsigned int)1, (unsigned int)1, dynamic_cast<phi::frameBuffer*>(phi::renderingSystem::defaultFrameBuffer));
    phi::renderingCommunicationBuffer::request(_bufferRequest);

    while (true)
    {
        _bufferRequest->mutex.lock();
        if (_bufferRequest->completed)
            break;

        _bufferRequest->mutex.unlock();
    }

    phi::renderingCommunicationBuffer::removeRequest(_bufferRequest);
    auto zBufferValue = _bufferRequest->result;
    _bufferRequest->mutex.unlock();
    DELETE(_bufferRequest);
    _bufferRequest = nullptr;

    if (zBufferValue == 1.0f)
    {
        _initialized = true;
        return;
    }

    auto camera = phi::scenesManager::get()->getScene()->getActiveCamera();
    auto proj = camera->getPerspProjMatrix();
    auto view = camera->getViewMatrix();
    auto mat = glm::inverse(proj * view);
    auto screenSize = phi::scenesManager::get()->getInfo().size;
    auto screenX = -1.0f + 2.0f * (_screenPos.x / screenSize.width);
    auto screenY = -1.0f + 2.0f * (_screenPos.y / screenSize.height);
    auto z = -1.0f + 2.0f * zBufferValue;

    auto pos4 = glm::vec4(screenX, -screenY, z, 1.0f);
    pos4 = mat * pos4;
    auto pos = glm::vec3(pos4.x / pos4.w, pos4.y / pos4.w, pos4.z / pos4.w);

    _object->setLocalPosition(pos);
    _initialized = true;
}

void addSceneObjectCommand::execute()
{
    init();

    auto scene = phi::scenesManager::get()->getScene();
    scene->add(_object);
}

void addSceneObjectCommand::executeUndo()
{
    auto scene = phi::scenesManager::get()->getScene();
    scene->remove(_object);
}