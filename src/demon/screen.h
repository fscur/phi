#pragma once
#include <phi.h>
#include <apps\window.h>
#include <apps\commandsManager.h>
#include <scenes\scene.h>
#include <ui\ui.h>

#include "defaultCameraController.h"
#include "library.h"

#include "layer.h"
#include "context.h"

#ifdef _DEBUG
#include <io/watcher.h>
#endif 
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
        library* _projectLibrary;

        context* _designContext;

        phi::ui* _ui;
        phi::node* _labelFps;
        phi::commandsManager* _commandsManager;
        defaultCameraController* _defaultController;
#ifdef _DEBUG
        phi::watcher* _watcher;
        phi::blockingQueue<phi::watcherMessage>* _messageQueue;
#endif 
    private:
        void initGL();
        void initLibraries();
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