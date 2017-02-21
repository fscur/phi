#include <precompiled.h>
#include "scene.h"

namespace phi
{
    scene::scene(camera* camera) :
        _camera(camera)
    {
    }

    scene::~scene()
    {
    }

    void scene::add(model* model)
    {
        _models.push_back(model);
    }

    vector<model*> scene::getModels()
    {
        return _models;
    }
}