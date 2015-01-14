#include "selectCommand.h"
#include "scenesManager.h"

namespace phi
{
	commandInfo* selectCommand::execute(commandInfo* info)
	{
		phi::ray ray = phi::scenesManager::get()->getScene()->getActiveCamera()->castRay(info->mousePos, info->viewportSize);
		std::vector<phi::sceneObject*>* sceneObjects = phi::scenesManager::get()->getScene()->getAllObjects();

		for (unsigned int i = 0; i < sceneObjects->size(); i++)
		{
			phi::sceneObject* sceneObject = (*sceneObjects)[i];

			sceneObject->setSelected(false);

			if (ray.intersects(sceneObject->getAabb()))
			sceneObject->setSelected(true);
		}

		return nullptr;
	}
}