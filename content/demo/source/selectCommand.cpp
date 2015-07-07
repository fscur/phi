#include "selectCommand.h"
#include "scenesManager.h"

namespace phi
{
    selectCommand::selectCommand()
    {
        _bufferRequest = nullptr;
        _pickRequest = nullptr;
    }

    selectCommand::~selectCommand()
    {
        DELETE(_pickRequest);
        DELETE(_bufferRequest);
    }

    void selectCommand::init()
    {
        _bufferRequest = new bufferRequest((unsigned int)info->mousePos.x, (unsigned int)info->mousePos.y, (unsigned int)1, (unsigned int)1, dynamic_cast<frameBuffer*>(renderingSystem::defaultFrameBuffer));
        renderingCommunicationBuffer::request(_bufferRequest);

        _pickRequest = new pickRequest((unsigned int)info->mousePos.x, (unsigned int)info->mousePos.y, (unsigned int)1, (unsigned int)1, renderingSystem::pickingFrameBuffer);
        renderingCommunicationBuffer::request(_pickRequest);
    }

    bool selectCommand::canExecute()
    {
        if (!_bufferRequest->mutex.try_lock())
            return false;

        if (!_bufferRequest->completed)
        {
            _bufferRequest->mutex.unlock();
            return false;
        }

        if (!_pickRequest->mutex.try_lock())
        {
            _bufferRequest->mutex.unlock();
            return false;
        }

        if (!_pickRequest->completed)
        {
            _bufferRequest->mutex.unlock();
            _pickRequest->mutex.unlock();
            return false;
        }

        renderingCommunicationBuffer::removeRequest(_bufferRequest);
        renderingCommunicationBuffer::removeRequest(_pickRequest);
        _zBufferValue = _bufferRequest->result;
        _id = _pickRequest->result;
        _bufferRequest->mutex.unlock();
        _pickRequest->mutex.unlock();
        DELETE(_bufferRequest);
        DELETE(_pickRequest);
        _bufferRequest = nullptr;
        _pickRequest = nullptr;

        return true;
    }

    void selectCommand::unselectAllSceneObjects()
    {
        std::vector<phi::sceneObject*>* sceneObjects = phi::scenesManager::get()->getScene()->getAllObjects();
        for (unsigned int i = 0; i < sceneObjects->size(); i++)
        {
            phi::sceneObject* sceneObject = (*sceneObjects)[i];
            sceneObject->setSelected(false);

            auto meshes = sceneObject->getSceneMeshes();
            for (unsigned int j = 0; j < meshes.size(); j++)
                meshes[j]->setIsSelected(false);
        }
    }

    void selectCommand::finish()
    {
        if (_zBufferValue == 1.0f)
        {
            unselectAllSceneObjects();
            return;
        }

        unsigned int meshId = _id >> 8;
        unsigned int objectId = _id & 255;
        sceneObject* selectedObject = scenesManager::get()->getScene()->getSceneObjectById(objectId);

        if (selectedObject != nullptr)
        {
            if (selectedObject->getSelected())
            {
                unselectAllSceneObjects();
                auto meshes = selectedObject->getSceneMeshes();
                meshes[meshId]->setIsSelected(true);
                selectedObject->setSelected(true);
            }
            else
            {
                unselectAllSceneObjects();
                auto meshes = selectedObject->getSceneMeshes();
                for (unsigned int j = 0; j < meshes.size(); j++)
                    meshes[j]->setIsSelected(true);
                selectedObject->setSelected(true);
            }
        }
    }
}