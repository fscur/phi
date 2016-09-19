#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\eventHandler.h>
#include <core\resolution.h>
#include <core\ray.h>

#undef near
#undef far

namespace phi
{
    class transform;

    class camera
    {
    private:
        resolution _resolution;
        float _near;
        float _far;
        float _fov;
        float _halfFovTangent;

        mat4 _projectionMatrix;
        mat4 _viewMatrix;
        transform* _transform;
        eventToken _transformChangedEventToken;

    private:
        void updateViewMatrix();
        void updateProjectionMatrix();
        void transformChanged(transform * sender);

    public:
        RENDERING_API camera(
            resolution resolution,
            float near,
            float far,
            float fov);

        RENDERING_API camera(
            resolution resolution,
            transform* transform,
            float near,
            float far,
            float fov);

        RENDERING_API virtual ~camera();

        RENDERING_API mat4 getViewMatrix();
        RENDERING_API mat4 getProjectionMatrix();
        RENDERING_API mat4 getInverseProjectionMatrix();
        RENDERING_API mat4 getViewProjectionMatrix();
        RENDERING_API void setResolution(const resolution& resolution);
        RENDERING_API void setFov(float value);
        RENDERING_API void setNear(float value);
        RENDERING_API void setFar(float value);

        RENDERING_API void moveTo(vec3 position);
        RENDERING_API void zoom(vec3 offset);
        RENDERING_API void orbit(vec3 origin, vec3 axisX, vec3 axisY, float angleX, float angleY);

        RENDERING_API vec3 screenPointToView(int mouseX, int mouseY, float depth);
        RENDERING_API ray screenPointToRay(int mouseX, int mouseY);
        RENDERING_API float zBufferToDepth(float zBufferValue);
        RENDERING_API vec3 castRayToPlane(int mouseX, int mouseY, plane plane);

        float getNear() const { return _near; }
        float getFar() const { return _far; }
        float getFov() const { return _fov; }
        float getHalfFovTangent() const { return _halfFovTangent; }

        resolution getResolution() const { return _resolution; }
        transform* getTransform() const { return _transform; }
    };
}