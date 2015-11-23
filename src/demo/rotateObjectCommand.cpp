#include "phi/demo/rotateObjectCommand.h"
#include "phi/core/mathUtils.h"

rotateObjectCommand::rotateObjectCommand(phi::object3D* object, glm::quat startOrientation, glm::quat endOrientation)
    : command()
{
    _object = object;
    _startOrientation = startOrientation;
    _endOrientation = endOrientation;
}

void rotateObjectCommand::execute()
{
    _object->setOrientation(_endOrientation);
}

void rotateObjectCommand::executeUndo()
{
    _object->setOrientation(_startOrientation);
}