#pragma once
#include <precompiled.h>
#include "rendering.h"
#include <core\component.h>

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
        transform* _transform;

    private:
        void updateViewMatrix();
        void updateProjectionMatrix();

    public:
        RENDERING_API camera(
            string name,
            float nearDistance,
            float farDistance,
            vec2 resolution,
            float fov,
            transform* transform);

        RENDERING_API inline mat4 getViewMatrix();
        RENDERING_API inline mat4 getProjectionMatrix();
        RENDERING_API inline float getZNear() const { return _near; }
        RENDERING_API inline float getZFar() const { return _far; }
        RENDERING_API inline float getAspect() const { return _aspect; }
        RENDERING_API inline vec2 getResolution() const { return _resolution; }
        RENDERING_API inline float getFov() const { return _fov; }
        RENDERING_API inline float getFocus() const { return _focus; }
        RENDERING_API inline transform* getTransform() { return _transform; }

        RENDERING_API void setResolution(vec2 value)
        {
            _resolution = value;
            _aspect = _resolution.x / _resolution.y;
            _changedProjection = true;
        }

        RENDERING_API void setTransform(transform* value)
        {
            _transform = value;
            _changedView = true;
        }

        RENDERING_API void moveTo(vec3 position);
        RENDERING_API void zoomIn(vec3 targetPos);
        RENDERING_API void zoomOut(vec3 targetPos);
        RENDERING_API void orbit(vec3 origin, vec3 axisX, vec3 axisY, float angleX, float angleY);

        RENDERING_API void update();
    };
}