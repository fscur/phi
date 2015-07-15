#ifndef _PHI_DIRECTIONAL_LIGHT_H_
#define _PHI_DIRECTIONAL_LIGHT_H_

#include "light.h"

namespace phi
{
    class directionalLight :
        public light
    {
    private:
        glm::mat4 _projectionMatrix;
        glm::mat4 _viewMatrix;
    private:
        void updateViewMatrix();

    public:
        SCENES_API directionalLight();
        SCENES_API directionalLight(glm::vec3 position, color color, float intensity, glm::vec3 direction);
        SCENES_API ~directionalLight();

        SCENES_API void update() override;

        SCENES_API glm::mat4 getViewMatrix() { return _viewMatrix; }
    };
}
#endif