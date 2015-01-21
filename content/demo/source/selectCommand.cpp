#include "selectCommand.h"
#include "scenesManager.h"

namespace phi
{
	commandInfo* selectCommand::execute(commandInfo* info)
	{
		std::vector<phi::sceneObject*>* sceneObjects = phi::scenesManager::get()->getScene()->getAllObjects();
		for (unsigned int i = 0; i < sceneObjects->size(); i++)
		{
			phi::sceneObject* sceneObject = (*sceneObjects)[i];

			sceneObject->setSelected(false);
		}

		if (renderingSystem::defaultFrameBuffer->getZBufferValue(info->mousePos) == 1.0f)
			return nullptr;

		unsigned int id = renderingSystem::pickingFrameBuffer->pick(info->mousePos);

		sceneObject* selectedObject = scenesManager::get()->getScene()->getSceneObjectById(id);

		if (selectedObject != nullptr && !selectedObject->getSelected())
			selectedObject->setSelected(true);

		return nullptr;
	}
}