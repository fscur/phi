#pragma once
#include <precompiled.h>
#include <core\size.h>
#include <rendering\renderer.h>
#include <scenes\scene.h>

#include "form.h"
#include "defaultCameraController.h"
#include "library.h"
#include "inputManager.h"

class screen :
    public form
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
    screen();
    ~screen(void);

    void onInitialize() override;
    void onResize(SDL_Event e) override;
    void onClosing() override;

    void onMouseDown(phi::mouseEventArgs* e) override;
    void onMouseMove(phi::mouseEventArgs* e) override;
    void onMouseUp(phi::mouseEventArgs* e) override;
    void onMouseWheel(phi::mouseEventArgs* e) override;
    void onKeyDown(phi::keyboardEventArgs* e) override;
    void onKeyUp(phi::keyboardEventArgs* e) override;

    void update() override;
    void render() override;

    void setResourcesPath(phi::string value) { _resourcesPath = value; }
    void setLibraryPath(phi::string value) { _libraryPath = value; }
};