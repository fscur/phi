#ifndef _PHI_DIRECTIONAL_LIGHT_H_
#define _PHI_DIRECTIONAL_LIGHT_H_

#include "phi/scenes/light.h"

namespace phi
{
    class directionalLight :
        public light
    {
    private:
        mat4 _projectionMatrix;
        mat4 _viewMatrix;
    private:
        void updateViewMatrix();

    public:
        SCENES_API directionalLight(vec3 position, color color, float intensity, vec3 direction);
        SCENES_API ~directionalLight();

        SCENES_API void update() override;

        SCENES_API mat4 getViewMatrix() { return _viewMatrix; }
    };
}
#endif