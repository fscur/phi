#pragma once
#include <phi.h>

#include <core\resolution.h>

#ifdef _DEBUG
#include <io/watcher.h>
#endif

#include <rendering\framebufferAllocator.h>

#include <application\window.h>
#include <application\commandsManager.h>

#include <context\layer.h>
#include <context\context.h>

#include <rendering\gl.h>

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
        phi::layer* _onDemandLayer;
        phi::layer* _constructionLayer;
        phi::layer* _nandinhoLayer;

        phi::camera* _sceneCamera;
        phi::camera* _nandinhoCamera;
        phi::camera* _constructionCamera;

        phi::node* _labelFps;
        phi::node* _labelNandinho;
        phi::node* _constructionLabel;
        phi::node* _chair0;

        phi::commandsManager* _commandsManager;
#ifdef _DEBUG
        phi::watcher* _watcher;
        phi::blockingQueue<phi::watcherMessage>* _messageQueue;
#endif

        phi::framebufferAllocator* _framebufferAllocator;

        phi::image* _translationImage;
        phi::image* _rotationImage;

    private:
        void initGL();
        void initWatcher();
        void initFramebuffers();
        void initLibraries();
        void openFileDialog();
        void initContexts();
        void initInput();
        void addSelectionUITo(phi::node* node, phi::font* font);
        phi::node* onDemandCreateUi();

        void onMouseDown(phi::mouseEventArgs* e);
        void onMouseUp(phi::mouseEventArgs* e);
        void onMouseClick(phi::mouseEventArgs* e);
        void onMouseDoubleClick(phi::mouseEventArgs* e);
        void onMouseMove(phi::mouseEventArgs* e);
        void onBeginMouseWheel(phi::mouseEventArgs* e);
        void onMouseWheel(phi::mouseEventArgs* e);
        void onEndMouseWheel(phi::mouseEventArgs* e);
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
        void onSwapBuffers() override;
        void onResize(phi::resolution resolution) override;
    };
}