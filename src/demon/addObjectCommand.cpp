#include <precompiled.h>
#include "addObjectCommand.h"

namespace demon
{
    using namespace phi;

    addObjectCommand::addObjectCommand(scene* scene, node* const objectToAdd) :
        _scene(scene),
        _objectToAdd(objectToAdd)
    {
    }

    addObjectCommand::~addObjectCommand()
    {
    }

    void addObjectCommand::execute()
    {
        _scene->add(_objectToAdd);
    }

    void addObjectCommand::executeUndo()
    {
        _scene->remove(_objectToAdd);
    }
}