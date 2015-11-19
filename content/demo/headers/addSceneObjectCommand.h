#ifndef _ADD_SCENE_OBJECT_COMMAND_H_
#define _ADD_SCENE_OBJECT_COMMAND_H_

#include <GLM\glm.hpp>
#include "command.h"
#include "sceneObject.h"
#include "renderingCommunicationBuffer.h"

class addSceneObjectCommand : public command
{
private:
    phi::sceneObject* _object;
    glm::vec2 _screenPos;
    phi::bufferRequest* _bufferRequest;
    bool _initialized;

public:
    addSceneObjectCommand(phi::sceneObject* object, glm::vec2 screenPos);

    void init();
    void execute() override;
    void executeUndo() override;
};

#endif