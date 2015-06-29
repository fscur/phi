#ifndef _PHI_DIRECTIONAL_LIGHT_H_
#define _PHI_DIRECTIONAL_LIGHT_H_

#include "light.h"

namespace phi
{
    class directionalLight :
        public light
    {
    private:
        glm::mat4 _orthoProjectionMatrix;
        glm::mat4 _viewProjectionMatrix;
    private:
        void updateViewMatrix();

    protected:
        void onPositionChanged() override;
        void onDirectionChanged() override;

    public:
        SCENES_API directionalLight();
        SCENES_API directionalLight(glm::vec3 position, color color, float intensity, glm::vec3 direction);
        SCENES_API ~directionalLight();

        SCENES_API transform* getTransform();
    };
}
#endif