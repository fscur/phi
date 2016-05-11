#pragma once
#include <phi.h>
#include <apps\window.h>
#include <apps\commandsManager.h>
//#include <ui\label.h>
#include <scenes\scene.h>
#include <ui\ui.h>

#include "defaultCameraController.h"
#include "library.h"

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
        library* _userLibrary;

        phi::ui* _ui;
        phi::commandsManager* _commandsManager;
        defaultCameraController* _defaultController;

    private:
        void initGL();
		void initAssimp();
        void initUserLibrary();
        void initScene();
        void initUi();
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