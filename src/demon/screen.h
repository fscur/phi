#pragma once
#include <phi.h>
#include <core\size.h>
#include <rendering\renderer.h>
#include <scenes\scene.h>

#include <apps\window.h>
#include "defaultCameraController.h"
#include "library.h"
#include "inputManager.h"

namespace demon
{
    class screen :
        public phi::window
    {
    private:
        bool _isMouseDown;
        phi::vec2 _mouseDownPos;
        phi::vec2 _lastMousePos;

        phi::gl* _gl;
        phi::scene* _scene;

        library* _library;

        commandsManager* _commandsManager;
        inputManager* _inputManager;
        defaultCameraController* _defaultController;

    private:
        void initGL();
        void initLibrary();
        void initScene();
        void initInput();

    public:
        screen(phi::string name, phi::uint witdh, phi::uint height);
        ~screen(void);

        void onInit() override;
        void onClosing() override;
        void onUpdate() override;
        void onRender() override;
        void onTick() override;
    };
}