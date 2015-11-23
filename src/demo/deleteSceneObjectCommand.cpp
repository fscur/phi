#include <phi/demo/deleteSceneObjectCommand.h>
#include <phi/scenes/scenesManager.h>

deleteSceneObjectCommand::deleteSceneObjectCommand()
    : command()
{
}

void deleteSceneObjectCommand::execute()
{
    auto scene = phi::scenesManager::get()->getScene();

    _objects = *(scene->getSelected());

    for (auto object : _objects)
    {
        scene->remove(object);
        object->setActive(false);
        object->setSelected(false);
    }
}

void deleteSceneObjectCommand::executeUndo()
{
    auto scene = phi::scenesManager::get()->getScene();

    for (auto object : _objects)
    {
        scene->add(object);
        object->setActive(true);
        object->setSelected(true);
    }
}