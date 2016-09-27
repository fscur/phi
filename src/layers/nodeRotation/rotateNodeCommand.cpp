#include <precompiled.h>

#include "rotateNodeCommand.h"

namespace phi
{
    rotateNodeCommand::rotateNodeCommand(node* node, vec3 originalPosition, vec3 targetPosition) :
        _node(node),
        _originalPosition(originalPosition),
        _targetPosition(targetPosition)
    {
    }

    void rotateNodeCommand::execute()
    {
        _node->getTransform()->setLocalPosition(_targetPosition);
    }

    void rotateNodeCommand::executeUndo()
    {
        _node->getTransform()->setLocalPosition(_originalPosition);
    }
}