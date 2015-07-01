#ifndef _PHI_CAMERA_EX_H_
#define _PHI_CAMERA_EX_H_

#include "scenes.h"
#include "object3D.h"
#include "frustum.h"

namespace phi
{
    class camera :
        public object3D
    {
    private:
        glm::mat4 _viewMatrix;
        frustum* _frustum;
        float _focus;

    public:
        SCENES_API camera(
            float zNear = 0.1f,
            float zFar = 100.0f,
            float aspect = 1.0f,
            float fov = 0.78539816f);

        SCENES_API glm::mat4 getViewMatrix() const { return _viewMatrix; }
        SCENES_API glm::mat4 getPerspProjMatrix() const { return _frustum->getPerspMatrix(); }
        SCENES_API frustum* getFrustum() const { return _frustum; }
        SCENES_API float getFocus() const { return _focus; }

        SCENES_API void setTarget(glm::vec3 value);

        SCENES_API void update() override;

        SCENES_API void moveTo(glm::vec3 position);
        SCENES_API void zoomIn(glm::vec3 targetPos);
        SCENES_API void zoomOut(glm::vec3 targetPos);
        SCENES_API void orbit(glm::vec3 origin, glm::vec3 axisX, glm::vec3 axisY, float angleX, float angleY);
    };
}

#endif