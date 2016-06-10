#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\resolution.h>
#include <core\node.h>
#include <core\component.h>
#include <core\eventHandler.h>
#include <core\ray.h>

#undef near
#undef far

namespace phi
{
    class transform;

    class camera :
        public component
    {
    private:
        resolution _resolution;
        float _near;
        float _far;
        float _fov;

        mat4 _projectionMatrix;
        mat4 _viewMatrix;
        bool _changedProjection;
        bool _changedView;
        eventToken _transformChangedEventToken;

    private:
        void updateViewMatrix();
        void updateProjectionMatrix();
        void transformChanged(transform* sender);

    protected:
        RENDERING_API void onNodeChanged(node* previousValue) override;

    public:
        RENDERING_API  camera(
            string name,
            resolution resolution,
            float near,
            float far,
            float fov);

        RENDERING_API virtual ~camera();

        RENDERING_API  mat4 getViewMatrix();
        RENDERING_API  mat4 getProjectionMatrix();
        RENDERING_API  transform* getTransform();

        RENDERING_API void setWidth(float value);
        RENDERING_API void setHeight(float value);
        RENDERING_API void setFov(float value);
        RENDERING_API void setNear(float value);
        RENDERING_API void setFar(float value);

        RENDERING_API void moveTo(vec3 position);
        RENDERING_API void zoom(vec3 offset);
        RENDERING_API void orbit(vec3 origin, vec3 axisX, vec3 axisY, float angleX, float angleY);

        RENDERING_API vec3 getWorldPositionRelativeToCamera(int mouseX, int mouseY, float zBufferValue);
        RENDERING_API float getWorldZRelativeToCamera(int mouseX, int mouseY, float zBufferValue);
        RENDERING_API ray screenPointToRay(float mouseX, float mouseY);

        float getNear() const { return _near; }
        float getFar() const { return _far; }
        float getFov() const { return _fov; }
        resolution getResolution() const { return _resolution; }

        static componentType getComponentType() { return componentType::CAMERA; }
    };
}