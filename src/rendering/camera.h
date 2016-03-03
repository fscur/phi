#pragma once
#include <phi.h>
#include "rendering.h"
#include <core\component.h>

#include <core\node.h>

namespace phi
{
    class camera :
        public component
    {
    private:
        mat4 _viewMatrix;
        mat4 _projectionMatrix;
        float _focus;
        bool _changedView;
        bool _changedProjection;
        bool _perspective;
        float _near;
        float _far;
        float _aspect;
        float _fov;
        vec2 _resolution;

    private:
        void updateViewMatrix();
        void updateProjectionMatrix();

    public:
        RENDERING_API camera(
            string name,
            float nearDistance,
            float farDistance,
            vec2 resolution,
            float fov);

        RENDERING_API mat4 getViewMatrix();
        RENDERING_API mat4 getProjectionMatrix();
        RENDERING_API float getZNear() const { return _near; }
        RENDERING_API float getZFar() const { return _far; }
        RENDERING_API float getAspect() const { return _aspect; }
        RENDERING_API vec2 getResolution() const { return _resolution; }
        RENDERING_API float getFov() const { return _fov; }
        RENDERING_API float getFocus() const { return _focus; }
        RENDERING_API transform* getTransform();

        RENDERING_API void setResolution(vec2 value);

        RENDERING_API void moveTo(vec3 position);
        RENDERING_API void zoomIn(vec3 targetPos);
        RENDERING_API void zoomOut(vec3 targetPos);
        RENDERING_API void orbit(vec3 origin, vec3 axisX, vec3 axisY, float angleX, float angleY);

        RENDERING_API void update();

        RENDERING_API static componentType getComponentType() { return componentType::CAMERA; }
    };
}