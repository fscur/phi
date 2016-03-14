#pragma once
#include <phi.h>
#include <core\size.h>
#include <rendering\renderer.h>
#include <scenes\scene.h>

#include <apps\window.h>
#include <apps\commandsManager.h>

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

        phi::commandsManager* _commandsManager;
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

        void setResourcesPath(phi::string value) { _resourcesPath = value; }
        void setLibraryPath(phi::string value) { _libraryPath = value; }
    };
}