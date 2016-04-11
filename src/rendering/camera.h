#pragma once
#include <phi.h>

#include <core\component.h>
#include <core\size.h>
#include <core\eventHandler.h>
#include <core\node.h>

#include "renderingApi.h"

namespace phi
{
    class transform;

    class camera :
        public component
    {
    private:
        mat4 _viewMatrix;
        mat4 _projectionMatrix;
        bool _changedView;
        bool _changedProjection;
        bool _perspective;
        float _near;
        float _far;
        float _aspect;
        float _fov;
        sizeui _resolution;
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
            float nearDistance,
            float farDistance,
            sizeui resolution,
            float fov);

        ~camera()
        {
            if (_node)
                _node->getTransform()->getChangedEvent()->unassign(_transformChangedEventToken);
        }

        RENDERING_API  mat4 getViewMatrix();
        RENDERING_API  mat4 getProjectionMatrix();

        RENDERING_API void setResolution(sizeui value);
        void setFov(float value) { _fov = value; _changedProjection = true; }
        void setZNear(float value) { _near = value; _changedProjection = true; }

        RENDERING_API void moveTo(vec3 position);
        RENDERING_API void zoom(vec3 offset);
        RENDERING_API void orbit(vec3 origin, vec3 axisX, vec3 axisY, float angleX, float angleY);

        RENDERING_API void update();

        transform* getTransform() const
        {
            if (_node == nullptr)
                return nullptr;

            return _node->getTransform();
        }

        float getZNear() const { return _near; }
        float getZFar() const { return _far; }
        float getAspect() const { return _aspect; }
        sizeui getResolution() const { return _resolution; }
        float getFov() const { return _fov; }

        static componentType getComponentType() { return componentType::CAMERA; }
    };
}