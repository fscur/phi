#pragma once
#include <phi.h>
#include <application\window.h>
#include <application\commandsManager.h>
#include <context\scene.h>

#include "defaultCameraController.h"
#include "library.h"

#include <context\layer.h>
#include <context\context.h>

#ifdef _DEBUG
#include <io/watcher.h>
#endif 
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
        screen(phi::string name, phi::uint witdh, phi::uint height);
        ~screen(void);

        void onInit() override;
        void onClosing() override;
        void onUpdate() override;
        void onRender() override;
        void onTick() override;
    };
}