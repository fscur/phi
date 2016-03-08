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
        bool _rotating;
        bool _shadowMap;

        float _translationSpeed;
        float _rotationSpeed;

        float _temp;

        phi::gl* _gl;
        phi::scene* _scene;
        phi::string _resourcesPath;
        phi::string _libraryPath;

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

        void onInitialize() override;
        void onClosing() override;

        void onUpdate() override;
        void onRender() override;

        void onMouseDown(phi::mouseEventArgs* e) override;
        void onMouseMove(phi::mouseEventArgs* e) override;
        void onMouseUp(phi::mouseEventArgs* e) override;
        void onMouseWheel(phi::mouseEventArgs* e) override;
        void onKeyDown(phi::keyboardEventArgs* e) override;
        void onKeyUp(phi::keyboardEventArgs* e) override;

        void setResourcesPath(phi::string value) { _resourcesPath = value; }
        void setLibraryPath(phi::string value) { _libraryPath = value; }
    };
}