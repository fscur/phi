#include "phi/demo/translateObjectCommand.h"

#include "phi/core/mathUtils.h"

translateObjectCommand::translateObjectCommand(phi::object3D* object, glm::vec3 startPos, glm::vec3 endPos) :
    command()
{
    _object = object;
    _startPos = startPos;
    _endPos = endPos;
}

void translateObjectCommand::execute()
{
    _object->setLocalPosition(_endPos);
}
void translateObjectCommand::executeUndo()
{
    _object->setLocalPosition(_startPos);
}