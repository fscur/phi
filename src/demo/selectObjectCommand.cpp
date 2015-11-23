#include "phi/demo/selectObjectCommand.h"

#include "phi/scenes/scenesManager.h"

selectObjectCommand::selectObjectCommand(phi::sceneObject* object, unsigned int* meshID)
    : command()
{
    _object = object;
    _meshID = meshID;
    _initialized = false;
}

void selectObjectCommand::init()
{
    _selectedObjects.clear();
    std::vector<phi::sceneObject*>* sceneObjects = phi::scenesManager::get()->getScene()->getAllObjects();
    for (unsigned int i = 0; i < sceneObjects->size(); i++)
    {
        phi::sceneObject* sceneObject = (*sceneObjects)[i];
        if (sceneObject->getSelected())
        {
            auto meshes = sceneObject->getSceneMeshes();
            for (unsigned int j = 0; j < meshes.size(); j++)
            {
                if (meshes[j]->getIsSelected())
                    _selectedObjects[sceneObject].push_back(meshes[j]);
            }
        }
    }

    _initialized = true;
}

void selectObjectCommand::unselectAllSceneObjects()
{
    std::vector<phi::sceneObject*>* sceneObjects = phi::scenesManager::get()->getScene()->getAllObjects();
    for (unsigned int i = 0; i < sceneObjects->size(); i++)
    {
        phi::sceneObject* sceneObject = (*sceneObjects)[i];
        if (sceneObject->getSelected())
        {
            sceneObject->setSelected(false);

            auto meshes = sceneObject->getSceneMeshes();
            for (unsigned int j = 0; j < meshes.size(); j++)
            {
                if (meshes[j]->getIsSelected())
                    meshes[j]->setIsSelected(false);
            }
        }
    }
}

void selectObjectCommand::execute()
{
    if (!_initialized)
        init();

    unselectAllSceneObjects();

    if (_object != nullptr)
    {
        if (_meshID != nullptr)
        {
            _object->setSelected(true);
            auto meshes = _object->getSceneMeshes();
            auto meshesCount = _object->getSceneMeshes().size();
            for (unsigned int i = 0; i < meshesCount; i++)
            {
                auto m = meshes[i];
                m->setIsSelected(false);
            }
            meshes[*_meshID]->setIsSelected(true);
        }
        else
            _object->setSelected(true);
    }
}

void selectObjectCommand::executeUndo()
{
    unselectAllSceneObjects();

    typedef std::map<phi::sceneObject*, std::vector<phi::sceneMesh*>>::iterator it_type;
    for(it_type iterator = _selectedObjects.begin(); iterator != _selectedObjects.end(); iterator++)
    {
        phi::sceneObject* sceneObject = iterator->first;
        sceneObject->setSelected(true);

        auto meshes = sceneObject->getSceneMeshes();
        for (unsigned int j = 0; j < meshes.size(); j++)
        {
            if (std::find(iterator->second.begin(), iterator->second.end(), meshes[j]) == iterator->second.end())
                meshes[j]->setIsSelected(false);
        }
    }
}