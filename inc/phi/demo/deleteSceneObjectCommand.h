#ifndef _DELETE_SCENE_OBJECT_COMMAND_H_
#define _DELETE_SCENE_OBJECT_COMMAND_H_

#include <phi/scenes/sceneObject.h>
#include <phi/demo/command.h>
#include <GLM\glm.hpp>

class deleteSceneObjectCommand : public command
{
private:
    std::vector<phi::sceneObject*> _objects;

public:
    deleteSceneObjectCommand();

    void execute() override;
    void executeUndo() override;
};

#endif