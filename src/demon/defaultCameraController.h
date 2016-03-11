#pragma once
#include <phi.h>
#include <scenes\scene.h>

#include "cameraController.h"
#include "shortcuts.h"

namespace demon
{
    class defaultCameraController :
        public cameraController
    {
    private:
        shortcuts _shortcuts;
        phi::ivec2 _lastMousePos;
        bool _rotating;
        bool _panning;
        GLfloat _zBufferValue;
        phi::vec3 _cameraPos;
        phi::vec3 _cameraRight;
        phi::vec3 _cameraUp;
        phi::ivec2 _startPos;
        float _eyeZ;
        phi::vec3 _targetPos;
        phi::scene* _scene;

    private:
        void executeInput(phi::inputKey key, phi::ivec2 mousePos);
        void initPan(phi::ivec2 mousePos);
        void initRotate(phi::ivec2 mousePos);
        void zoom(phi::ivec2 mousePos, bool in);
        void pan(phi::ivec2 mousePos);
        void rotate(phi::ivec2 mousePos);

    public:
        defaultCameraController(phi::scene* scene);
        virtual ~defaultCameraController();

        virtual bool onMouseDown(phi::mouseEventArgs* e, phi::inputKey key) override;
        virtual bool onMouseMove(phi::mouseEventArgs* e) override;
        virtual bool onMouseUp(phi::mouseEventArgs* e, phi::inputKey key) override;
        virtual bool onMouseWheel(phi::mouseEventArgs* e, phi::inputKey key) override;

        virtual void update() override;
    };
}