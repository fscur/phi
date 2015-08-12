#ifndef _TRANSLATE_OBJECT_COMMAND_H_
#define _TRANSLATE_OBJECT_COMMAND_H_

#include <glm\glm.hpp>
#include "object3D.h"
#include "command.h"

class translateObjectCommand :
    public command
{
private:
    phi::object3D* _object;
    glm::vec3 _startPos;
    glm::vec3 _endPos;

protected:
    void execute() override;
    void executeUndo() override;

public:
    translateObjectCommand(phi::object3D* object, glm::vec3 startPos, glm::vec3 endPos);
};

#endif