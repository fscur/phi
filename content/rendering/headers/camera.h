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
        RENDERING_API camera(
            float zNear = 0.1f, 
            float zFar = 100.0f, 
            float aspect = 1.0f, 
            float fov = 0.78539816f,
            float focusDistance = 1.0f);

        RENDERING_API ~camera(void);

        RENDERING_API glm::mat4 getViewMatrix() const { return _viewMatrix; }
        RENDERING_API glm::mat4 getPerspProjMatrix() const { return _frustum->getPerspMatrix(); }
        RENDERING_API glm::mat4 getOrthoProjMatrix() const { return _orthoProjMatrix; }

        RENDERING_API frustum* getFrustum() const { return _frustum; }
        RENDERING_API void setFrustum(frustum* frustum);

        RENDERING_API glm::vec3 getTarget() const { return _target; }
        RENDERING_API void setTarget(glm::vec3 target);

        RENDERING_API float getFocus() const { return _focus; }
        RENDERING_API void setFocus(float focus);

        RENDERING_API bool getChanged() const override { return _changed || _frustum->getChanged(); }

        RENDERING_API bool init();
        RENDERING_API void update();
        RENDERING_API void debugRender() override;

        RENDERING_API void dolly(float amount);
        RENDERING_API void zoomIn();
        RENDERING_API void zoomOut();
        RENDERING_API void orbit(glm::vec3 origin, glm::vec3 axis, float angle);

        RENDERING_API ray castRay(glm::vec2 screenCoords, size<float> screenSize);
    };
}
#endif

