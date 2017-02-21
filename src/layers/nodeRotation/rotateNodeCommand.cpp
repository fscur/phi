#include <precompiled.h>

#include "rotateNodeCommand.h"

namespace phi
{
    rotateNodeCommand::rotateNodeCommand(node* node, quat originalOrientation, quat targetOrientation) :
        _node(node),
        _originalOrientation(originalOrientation),
        _targetOrientation(targetOrientation)
    {
    }

    void rotateNodeCommand::execute()
    {
        _node->getTransform()->setLocalOrientation(_targetOrientation);
    }

    void rotateNodeCommand::executeUndo()
    {
        _node->getTransform()->setLocalOrientation(_originalOrientation);
    }
}