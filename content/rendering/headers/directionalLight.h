#ifndef _PHI_DIRECTIONAL_LIGHT_H_
#define _PHI_DIRECTIONAL_LIGHT_H_

#include "light.h"

namespace phi
{
    class directionalLight :
        public light
    {
    private:
        glm::vec3 _direction;
        glm::mat4 _orthoProjectionMatrix;
        glm::mat4 _viewProjectionMatrix;
    private:
        void updateViewMatrix();
    public:
        RENDERING_API directionalLight();
        RENDERING_API directionalLight(glm::vec3 position, color color, float intensity, glm::vec3 direction);
        RENDERING_API ~directionalLight();

        RENDERING_API transform* getTransform();

        RENDERING_API glm::vec3 getDirection();
        RENDERING_API void setDirection(glm::vec3 direction);

        RENDERING_API void setPosition(glm::vec3 position) override;
    };
}
#endif