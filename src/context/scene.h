#pragma once
#include <phi.h>

#include <core/model.h>

#include <rendering/camera.h>

#include "contextApi.h"

namespace phi
{
    class scene
    {
    private:
        camera* _camera;
        vector<model*> _models;

    public:
        CONTEXT_API scene(camera* camera);
        CONTEXT_API ~scene();

        CONTEXT_API void add(model* model);

        CONTEXT_API vector<model*> getModels();

        camera* getCamera() const { return _camera; }
        void setCamera(camera* camera) { _camera = camera; }
    };
}