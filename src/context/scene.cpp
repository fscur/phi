#include <precompiled.h>
#include "scene.h"

namespace phi
{
    scene::scene(layer* layer, camera* camera) :
        _layer(layer),
        _camera(camera)
    {
    }

    scene::~scene()
    {
        safeDelete(_layer);
    }

    void scene::add(model* model)
    {
        _models.push_back(model);
        _layer->add(model->getNode());
    }
}