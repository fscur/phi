#pragma once
#include <phi.h>

#include <core/model.h>

#include <rendering/camera.h>

#include "contextApi.h"
#include "layer.h"

namespace phi
{
    class scene
    {
    private:
        layer* _layer;
        camera* _camera;

        vector<model*> _models;

    public:
        CONTEXT_API scene(layer* layer, camera* camera);
        CONTEXT_API ~scene();

        CONTEXT_API void add(model* model);

        CONTEXT_API vector<model*> getModels();
        CONTEXT_API camera* getCamera();
    };
}