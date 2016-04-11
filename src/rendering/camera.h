#pragma once
#include <phi.h>

#include <core\component.h>
#include <core\size.h>
#include <core\eventHandler.h>

#include "renderingApi.h"

#undef near
#undef far

namespace phi
{
    class transform;

    class camera :
        public component
    {
    private:
        float _width;
        float _height;
        float _near;
        float _far;
        float _fov;
        float _aspect;
        float _focus;
        mat4 _projectionMatrix;
        mat4 _viewMatrix;
        bool _changedProjection;
        bool _changedView;
        eventToken _transformChangedEventToken;

    private:
        void updateViewMatrix();
        void updateProjectionMatrix();
        void transformChanged();

    protected:
        RENDERING_API void onNodeChanged(node* previousValue) override;

    public:
        RENDERING_API  camera(
            string name,
            float width,
            float height,
            float near,
            float far,
            float fov);

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

        float getNear() const { return _near; }
        float getFar() const { return _far; }
        float getAspect() const { return _aspect; }
        float getWidth() const { return _width; }
        float getHeight() const { return _height; }
        float getFov() const { return _fov; }
        float getFocus() const { return _focus; }

        static componentType getComponentType() { return componentType::CAMERA; }
    };
}