#ifndef PHI_ROTATION_CONTROL_H_
#define PHI_ROTATION_CONTROL_H_

#include "control.h"
#include "object3D.h"
#include "lineMesh.h"
#include "shader.h"
#include "camera.h"
#include "rotationEventArgs.h"

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
        glm::mat4 _modelMatrix;
        glm::mat4 _xModelMatrix;
        glm::mat4 _yModelMatrix;
        glm::mat4 _zModelMatrix;
        std::vector<glm::vec3> _xPositions;
        std::vector<glm::vec3> _yPositions;
        std::vector<glm::vec3> _zPositions;
        bool _mouseOverX;
        bool _mouseOverY;
        bool _mouseOverZ;
        bool _clickedOverX;
        bool _clickedOverY;
        bool _clickedOverZ;
        glm::vec2 _mouseStartPos;
        float _currentAngle;
        glm::quat _startOrientation;
        eventHandler<rotationEventArgs>* _rotationFinished;

    private:
        void createCircleMesh();
        void updateModelMatrix();
        void renderCircle(color color, glm::mat4 modelMatrix);

        virtual void onMouseDown(mouseEventArgs* e) override;
        virtual void onMouseUp(mouseEventArgs* e) override;
        virtual void onMouseMove(mouseEventArgs* e) override;
        virtual void onMouseLeave(mouseEventArgs* e) override;

    public:
        UI_API rotationControl(size<GLuint> viewportSize);
        UI_API ~rotationControl();

        UI_API eventHandler<rotationEventArgs>* getRotationFinished() { return _rotationFinished; }

        UI_API void setCamera(camera* value) { _camera = value; }

        UI_API void attachTo(object3D* object);

        UI_API virtual bool isPointInside(int x, int y) override;
        UI_API void render() override;
    };
}

#endif