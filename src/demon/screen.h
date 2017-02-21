#pragma once
#include <phi.h>

#include <core/resolution.h>

#ifdef _DEBUG
#include <io/watcher.h>
#endif

#include <rendering/framebufferAllocator.h>

#include <application/window.h>
#include <application/commandsManager.h>

#include <layers/layer.h>
#include <context/context.h>
#include <context/scene.h>

#include <rendering/gl.h>

#include  <layers/nodeTranslation/translationInputController.h>
#include <layers/nodeRotation/rotationInputController.h>
#include <layers/nodeSelection/selectionLayerBehaviour.h>

#include <domain/library.h>
#include <domain/repositories/iProjectRepository.h>

namespace demon
{
    class screen :
        public phi::window
    {
    private:
        library* _userLibrary;
        library* _projectLibrary;
        project* _project;
        
        phi::scene* _scene;
        phi::gl* _gl;

        phi::font* _font;

        iProjectRepository* _projectRepository;

        phi::context* _designContext;
        phi::context* _buildingContext;
        phi::context* _activeContext;

        phi::layer* _sceneLayer;
        //phi::layer* _onDemandLayer;
        phi::layer* _buildingLayer;
        phi::layer* _designLayer;

        phi::camera* _sceneCamera;
        phi::camera* _designLayerCamera;
        phi::camera* _buildingLayerCamera;

        phi::node* _labelFps;
        phi::node* _changeContextButton;

        phi::commandsManager* _commandsManager;

        phi::framebufferAllocator* _framebufferAllocator;

        phi::translationInputController* _translationController;
        phi::rotationInputController* _rotationController;
        phi::selectionLayerBehaviour* _selectionBehaviour;

        phi::node* _onDemandUi;
        phi::image* _translationImage;
        phi::image* _rotationImage;

#ifdef _DEBUG
        phi::watcher* _watcher;
        phi::blockingQueue<phi::watcherMessage>* _messageQueue;
#endif

    private:
        void initGL();
        void initWatcher();
        void initFramebuffers();
        void initLibraries();
        void initInput();

        void initSceneLayer();
        void initDesignLayer();
        void initBuildingLayer();
        void initContexts();
        
        void initScene();
        void loadProject();
        void saveProject();

        phi::node* createOnDemandUiNode();
        void showOnDemandUi();
        void hideOnDemandUi();
        void onNodeSelectionChanged(phi::node* node);

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