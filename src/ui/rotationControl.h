#ifndef PHI_ROTATION_CONTROL_H_
#define PHI_ROTATION_CONTROL_H_

#include "phi/ui/control.h"
#include "phi/ui/rotationEventArgs.h"

#include <phi/core/object3D.h>

#include <phi/rendering/lineMesh.h>
#include <phi/rendering/shader.h>

#include <phi/scenes/camera.h>

namespace phi
{
    class rotationControl :
        public control
    {
    private:
        object3D* _object;
        lineMesh* _circleMesh;
        shader* _shader;
        camera* _camera;
        color _xColor;
        color _yColor;
        color _zColor;
        mat4 _modelMatrix;
        mat4 _xModelMatrix;
        mat4 _yModelMatrix;
        mat4 _zModelMatrix;
        std::vector<vec3> _xPositions;
        std::vector<vec3> _yPositions;
        std::vector<vec3> _zPositions;
        bool _mouseOverX;
        bool _mouseOverY;
        bool _mouseOverZ;
        bool _clickedOverX;
        bool _clickedOverY;
        bool _clickedOverZ;
        vec2 _mouseStartPos;
        float _currentAngle;
        quat _startOrientation;
        eventHandler<rotationEventArgs*>* _rotating;
        eventHandler<rotationEventArgs>* _rotationFinished;

    private:
        void createCircleMesh();
        void updateModelMatrix();
        void renderCircle(color color, mat4 modelMatrix);

        virtual void onMouseDown(mouseEventArgs* e) override;
        virtual void onMouseUp(mouseEventArgs* e) override;
        virtual void onMouseMove(mouseEventArgs* e) override;
        virtual void onMouseLeave(mouseEventArgs* e) override;

    public:
        UI_API rotationControl(sizef viewportSize);

        UI_API eventHandler<rotationEventArgs*>* getRotating() const { return _rotating; }
        UI_API eventHandler<rotationEventArgs>* getRotationFinished() { return _rotationFinished; }

        UI_API void setCamera(camera* value) { _camera = value; }

        UI_API void attachTo(object3D* object);

        UI_API virtual bool isPointInside(int x, int y) override;
        UI_API void onRender() override;
    };
}

#endif