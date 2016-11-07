#include <precompiled.h>
#include "project.h"

namespace demon
{
    using namespace phi;

    project::project(scene* scene) :
        _scene(scene)
    {
    }

    project::~project()
    {
        safeDelete(_scene);
    }

    scene* project::getScene()
    {
        return _scene;
    }
}