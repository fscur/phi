#pragma once
#include <phi.h>

#include <core\node.h>

#include "scenesApi.h"
#include "light.h"

namespace phi
{
    class transform;

    class directionalLight :
        public light
    {
    private:
        mat4 _projectionMatrix;
        mat4 _viewMatrix;
        bool _changed;

    private:
        void updateViewMatrix();
        transform* getTransform() const
        {
            if (_node == nullptr)
                return nullptr;

            return _node->getTransform();
        }

    public:
        SCENES_API directionalLight(string name, color color, float intensity);
        SCENES_API ~directionalLight();

        SCENES_API mat4 getViewMatrix();
    };
}