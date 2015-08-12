#ifndef _ROTATE_OBJECT_COMMAND_H_
#define _ROTATE_OBJECT_COMMAND_H_

#include <glm\glm.hpp>
#include "object3D.h"
#include "command.h"

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