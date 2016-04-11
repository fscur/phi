#pragma once
#include <phi.h>

#include <core\transform.h>

#include <rendering\lineMesh.h>
#include <rendering\shader.h>
#include <rendering\camera.h>


#include "control.h"
#include "rotationEventArgs.h"

namespace phi
{
    class rotationControl :
        public control
    {
    private:
        transform* _transform;
        lineMesh* _circleMesh;
        shader* _shader;
        camera* _camera;
        eventHandler<rotationEventArgs*>* _rotating;
        eventHandler<rotationEventArgs>* _rotationFinished;
        color _xColor;
        color _yColor;
        color _zColor;
        mat4 _modelMatrix;
        mat4 _xModelMatrix;
        mat4 _yModelMatrix;
        mat4 _zModelMatrix;
        vec2 _mouseStartPos;
        quat _startOrientation;
        float _currentAngle;
        vector<vec3> _xPositions;
        vector<vec3> _yPositions;
        vector<vec3> _zPositions;
        bool _clickedOverX;
        bool _clickedOverY;
        bool _clickedOverZ;
        bool _mouseOverX;
        bool _mouseOverY;
        bool _mouseOverZ;

    private:
        void createCircleMesh();
        void updateModelMatrix();
        void renderCircle(color color, mat4 modelMatrix);

        virtual void onMouseDown(mouseEventArgs* e) override;
        virtual void onMouseUp(mouseEventArgs* e) override;
        virtual void onMouseMove(mouseEventArgs* e) override;
        virtual void onMouseLeave(mouseEventArgs* e) override;

    public:
        UI_API rotationControl(sizeui viewportSize);

        UI_API void attachTo(transform* transform);

        UI_API virtual bool isPointInside(int x, int y) override;
        UI_API void onRender() override;

        eventHandler<rotationEventArgs*>* getRotating() const { return _rotating; }
        eventHandler<rotationEventArgs>* getRotationFinished() { return _rotationFinished; }
        void setCamera(camera* value) { _camera = value; }
    };
}