#include <precompiled.h>

#include "translateNodeCommand.h"

namespace phi
{
    translateNodeCommand::translateNodeCommand(node* node, vec3 originalPosition, vec3 targetPosition) :
        _node(node),
        _originalPosition(originalPosition),
        _targetPosition(targetPosition)
    {
    }

    void translateNodeCommand::execute()
    {
        _node->getTransform()->setLocalPosition(_targetPosition);
    }

    void translateNodeCommand::executeUndo()
    {
        _node->getTransform()->setLocalPosition(_originalPosition);
    }
}