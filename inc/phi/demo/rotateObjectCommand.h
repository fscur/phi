#ifndef _ROTATE_OBJECT_COMMAND_H_
#define _ROTATE_OBJECT_COMMAND_H_

#include "phi/core/object3D.h"
#include "phi/demo/command.h"

#include <glm\glm.hpp>

class rotateObjectCommand :
    public command
{
private:
    phi::object3D* _object;
    glm::quat _startOrientation;
    glm::quat _endOrientation;

protected:
    void execute() override;
    void executeUndo() override;

public:
    rotateObjectCommand(phi::object3D* object, glm::quat startOrientation, glm::quat endOrientation);
};

#endif