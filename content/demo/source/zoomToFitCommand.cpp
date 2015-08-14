#include <glm/gtx/constants.hpp>
#include "zoomToFitCommand.h"
#include "scenesManager.h"

zoomToFitCommand::zoomToFitCommand()
{
}

void zoomToFitCommand::execute()
{
    phi::sceneObject* object = nullptr;
    phi::camera* camera = phi::scenesManager::get()->getScene()->getActiveCamera();

    auto allObjects = phi::scenesManager::get()->getScene()->getAllObjects();
    for (auto i = 0; i < allObjects->size();i++)
    {
        object = allObjects->at(i);
        if (object->getSelected())
            break;
    }

    if (object == nullptr)
        return;

    auto pos = object->getPosition();
    auto min = phi::mathUtils::multiply(object->getModelMatrix(), object->getAabb()->getMin());
    auto max = phi::mathUtils::multiply(object->getModelMatrix(), object->getAabb()->getMax());
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