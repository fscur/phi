#ifndef _PHI_CAMERA_H_
#define _PHI_CAMERA_H_

#include "sceneObject.h"
#include "ray.h"
#include "frustum.h"

namespace phi
{
    class camera :
        public sceneObject
    {
    private:
        float _znear;
        float _zfar;
        float _aspect;
        float _fov;
        float _focus;
        glm::mat4 _orthoProjMatrix;
        glm::mat4 _viewMatrix;
        glm::vec3 _target;
        float _zoomFactor;
        frustum* _frustum;

    private:
        void updateCoordinateSystem();
    public:
        SCENES_API camera(
            float zNear = 0.1f, 
            float zFar = 100.0f, 
            float aspect = 1.0f, 
            float fov = 0.78539816f,
            float focusDistance = 1.0f);

        SCENES_API ~camera(void);

        SCENES_API glm::mat4 getViewMatrix() const { return _viewMatrix; }
        SCENES_API glm::mat4 getPerspProjMatrix() const { return _frustum->getPerspMatrix(); }
        SCENES_API glm::mat4 getOrthoProjMatrix() const { return _orthoProjMatrix; }

        SCENES_API frustum* getFrustum() const { return _frustum; }
        SCENES_API void setFrustum(frustum* frustum);

        SCENES_API glm::vec3 getTarget() const { return _target; }
        SCENES_API void setTarget(glm::vec3 target);

        SCENES_API float getFocus() const { return _focus; }
        SCENES_API void setFocus(float focus);

        SCENES_API bool getChanged() const override { return _changed || _frustum->getChanged(); }

        SCENES_API bool init();
        SCENES_API void update();
        SCENES_API void debugRender() override;

        SCENES_API void dolly(float amount);
        SCENES_API void zoomIn();
        SCENES_API void zoomOut();
        SCENES_API void orbit(glm::vec3 origin, glm::vec3 axis, float angle);

        SCENES_API ray castRay(glm::vec2 screenCoords, size<float> screenSize);
    };
}
#endif

