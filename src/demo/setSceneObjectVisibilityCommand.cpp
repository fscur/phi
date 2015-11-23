#include "phi/demo/setSceneObjectVisibilityCommand.h"

setSceneObjectVisibilityCommand::setSceneObjectVisibilityCommand(phi::sceneObject* sceneObject, bool visibility)
    : command()
{
    _sceneObject = sceneObject;
    _visibility = visibility;
}

void setSceneObjectVisibilityCommand::execute()
{
    _sceneObject->setActive(_visibility);
}

void setSceneObjectVisibilityCommand::executeUndo()
{
    _sceneObject->setActive(!_visibility);
}