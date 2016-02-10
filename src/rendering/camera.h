#ifndef _PHI_CAMERA_H_
#define _PHI_CAMERA_H_

#include <phi/core/object3D.h>
#include <phi/rendering/frustum.h>

namespace phi
{
    class camera :
        public object3D
    {
    private:
        mat4 _viewMatrix;
        frustum* _frustum; //TODO: pointer ?? Reference!
        float _focus;

    public:
        RENDERING_API camera(
            float nearDistance,
            float farDistance,
            sizef resolution,
            float fov);

        RENDERING_API inline mat4 getViewMatrix() const { return _viewMatrix; }
        RENDERING_API inline mat4 getProjectionMatrix() const { return _frustum->getPerspectiveMatrix(); }
        RENDERING_API inline sizef getResolution() const { return _frustum->getResolution(); }
        RENDERING_API inline frustum* getFrustum() const { return _frustum; }
        RENDERING_API inline float getFocus() const { return _focus; }

        RENDERING_API void setTarget(vec3 value);
        RENDERING_API void setResolution(sizef value) { _frustum->setResolution(value); }
        RENDERING_API void update() override;

        RENDERING_API void moveTo(vec3 position);
        RENDERING_API void zoomIn(vec3 targetPos);
        RENDERING_API void zoomOut(vec3 targetPos);
        RENDERING_API void orbit(vec3 origin, vec3 axisX, vec3 axisY, float angleX, float angleY);
    };
}

#endif