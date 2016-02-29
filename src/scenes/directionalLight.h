#pragma once

#include "light.h"

namespace phi
{
    class directionalLight :
        public light
    {
    private:
        mat4 _projectionMatrix;
        mat4 _viewMatrix;
        bool _changed;

    private:
        void updateViewMatrix();

    public:
        SCENES_API directionalLight(std::string name, color color, float intensity, transform* transform);
        SCENES_API ~directionalLight();

        SCENES_API mat4 getViewMatrix();
    };
}