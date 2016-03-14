#pragma once
#include <phi.h>
#include <scenes\scene.h>

#include "cameraController.h"

namespace demon
{
    class defaultCameraController :
        public cameraController
    {
    private:
        float _lastMousePosX;
        float _lastMousePosY;
        bool _rotating;
        bool _panning;
        GLfloat _zBufferValue;
        phi::vec3 _cameraPos;
        phi::vec3 _cameraRight;
        phi::vec3 _cameraUp;
        float _startPosX;
        float _startPosY;
        float _eyeZ;
        phi::vec3 _targetPos;
        phi::scene* _scene;

    private:
        void initPan(float mouseX, float mouseY);
        void initRotate(float mouseX, float mouseY);
        void zoom(float mouseX, float mouseY, bool in);
        void pan(float mouseX, float mouseY);
        void rotate(float mouseX, float mouseY);

        virtual void onMouseDown(phi::mouseEventArgs* e) override;
        virtual void onMouseMove(phi::mouseEventArgs* e) override;
        virtual void onMouseUp(phi::mouseEventArgs* e) override;
        virtual void onMouseWheel(phi::mouseEventArgs* e) override;

    public:
        defaultCameraController(phi::scene* scene);

        virtual void update() override;
    };
}