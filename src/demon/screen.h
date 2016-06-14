#pragma once
#include <phi.h>

#include <core\resolution.h>

#ifdef _DEBUG
#include <io/watcher.h>
#endif

#include <application\window.h>
#include <application\commandsManager.h>

#include <context\layer.h>
#include <context\context.h>

#include <rendering\gl.h>

#include "defaultCameraController.h"
#include "library.h"

namespace demon
{
    class screen :
        public phi::window
    {
    private:
        phi::gl* _gl;
        library* _userLibrary;
        library* _projectLibrary;

        phi::context* _designContext;
        phi::context* _constructionContext;
        phi::context* _activeContext;

        phi::layer* _sceneLayer;
        phi::layer* _constructionLayer;
        phi::layer* _nandinhoLayer;

        phi::node* _labelFps;
        phi::commandsManager* _commandsManager;
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

        void onMouseDown(phi::mouseEventArgs* e);
        void onMouseMove(phi::mouseEventArgs* e);
        void onMouseUp(phi::mouseEventArgs* e);
        void onMouseWheel(phi::mouseEventArgs* e);
        void onKeyDown(phi::keyboardEventArgs* e);
        void onKeyUp(phi::keyboardEventArgs* e);

    public:
        screen(phi::wstring title, phi::resolution resolution);
        ~screen(void);

        void onInit() override;
        void onClosing() override;
        void onUpdate() override;
        void onRender() override;
        void onTick() override;
        void onResize(phi::resolution resolution) override;
    };
}