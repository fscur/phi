#include <glm/gtx/constants.hpp>
#include "zoomToFitCommand.h"
#include "scenesManager.h"

zoomToFitCommand::zoomToFitCommand()
{
}

void zoomToFitCommand::execute()
{
    phi::sceneObject* selectedObject = nullptr;
    auto scene = phi::scenesManager::get()->getScene();
    auto camera = scene->getActiveCamera();

    auto allObjects = phi::scenesManager::get()->getScene()->getAllObjects();
    for (auto i = 0; i < allObjects->size();i++)
    {
        auto object = allObjects->at(i);
        if (object->getSelected())
        {
            selectedObject = object;
            break;
        }
    }

    glm::vec3 min;
    glm::vec3 max;

    if (selectedObject == nullptr)
    {
        min = scene->getAabb()->getMin();
        max = scene->getAabb()->getMax();
    }
    else
    {
        min = phi::mathUtils::multiply(selectedObject->getModelMatrix(), selectedObject->getAabb()->getMin());
        max = phi::mathUtils::multiply(selectedObject->getModelMatrix(), selectedObject->getAabb()->getMax());
    }

    auto fov = camera->getFrustum()->getFov();

    auto p = (max + min) * 0.5f;
    auto radius = glm::length(max - min) * 0.5f;
    auto angle = glm::pi<float>() * 0.5f - fov * 0.5f;

    auto a = glm::sin(angle) * radius;
    auto b = glm::cos(angle) * radius;
    auto c = a / glm::tan(fov * 0.5f);

    auto d = b + c;

    auto dir = -glm::normalize(camera->getDirection());
    auto zNear = camera->getFrustum()->getZNear();
    camera->setLocalPosition(p + (dir * ((float)d + zNear)));
}