#include "phi/demo/selectAtPositionCommand.h"

#include "phi/scenes/scenesManager.h"

selectAtPositionCommand::selectAtPositionCommand(glm::vec2 position)
    : command()
{
    _position = position;
    _bufferRequest = nullptr;
    _pickRequest = nullptr;
    _initialized = false;
    _selectCommand = nullptr;
}

void selectAtPositionCommand::init()
{
    if (_initialized)
        return;

    _bufferRequest = new phi::bufferRequest((unsigned int)_position.x, (unsigned int)_position.y, (unsigned int)1, (unsigned int)1, dynamic_cast<phi::frameBuffer*>(phi::renderingSystem::defaultFrameBuffer));
    phi::renderingCommunicationBuffer::request(_bufferRequest);

    _pickRequest = new phi::pickRequest((unsigned int)_position.x, (unsigned int)_position.y, (unsigned int)1, (unsigned int)1, phi::renderingSystem::pickingFrameBuffer);
    phi::renderingCommunicationBuffer::request(_pickRequest);

    while (true)
    {
        _bufferRequest->mutex.lock();
        if (_bufferRequest->completed)
            break;

        _bufferRequest->mutex.unlock();
    }

    while (true)
    {
        _pickRequest->mutex.lock();
        if (_pickRequest->completed)
            break;

        _pickRequest->mutex.unlock();
    }

    phi::renderingCommunicationBuffer::removeRequest(_bufferRequest);
    phi::renderingCommunicationBuffer::removeRequest(_pickRequest);
    _zBufferValue = _bufferRequest->result;
    auto id = _pickRequest->result;
    _bufferRequest->mutex.unlock();
    _pickRequest->mutex.unlock();
    safeDelete(_bufferRequest);
    safeDelete(_pickRequest);
    _bufferRequest = nullptr;
    _pickRequest = nullptr;

    if (_zBufferValue == 1.0f)
    {
        _selectCommand = new selectObjectCommand(nullptr, nullptr);
        _initialized = true;
        return;
    }

    _meshId = id >> 8;
    _objectId = id & 255;
    phi::sceneObject* object = phi::scenesManager::get()->getScene()->getSceneObjectById(_objectId);
    if (object->getSelected())
        _selectCommand = new selectObjectCommand(object, &_meshId);
    else
        _selectCommand = new selectObjectCommand(object, nullptr);
    _initialized = true;
}

void selectAtPositionCommand::execute()
{
    init();

    _selectCommand->start();
}

void selectAtPositionCommand::executeUndo()
{
    init();

    _selectCommand->startUndo();
}