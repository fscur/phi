#include <precompiled.h>
#include "deleteObjectCommand.h"

namespace demon
{
    using namespace phi;

    deleteObjectCommand::deleteObjectCommand(scene* scene, selectionMouseController* selectionMouseController) :
        _scene(scene),
        _selectionMouseController(selectionMouseController)
    {
    }

    deleteObjectCommand::~deleteObjectCommand()
    {
    }

    void deleteObjectCommand::execute()
    {
    }
}