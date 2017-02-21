#pragma once
#include <phi.h>
#include "layersApi.h"
#include "layer.h"

#include </scene.h>

namespace phi
{
    class sceneLayer :
        public layer
    {
    public:
        sceneLayer(scene* scene);

    private:
        scene* _scene;
    };
}