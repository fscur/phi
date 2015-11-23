#ifndef _ADD_SCENE_OBJECT_COMMAND_H_
#define _ADD_SCENE_OBJECT_COMMAND_H_

#include "phi/rendering/renderingCommunicationBuffer.h"

#include "phi/scenes/sceneObject.h"

#include "phi/demo/command.h"

#include <GLM\glm.hpp>

class addSceneObjectCommand : public command
{
private:
    phi::sceneObject* _object;
    glm::vec2 _screenPos;
    phi::bufferRequest* _bufferRequest;
    bool _initialized;

public:
    addSceneObjectCommand(phi::sceneObject* object, glm::vec2 screenPos);
    addSceneObjectCommand(phi::sceneObject* object);

    void init();
    void execute() override;
    void executeUndo() override;
};

#endif