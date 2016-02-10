#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <phi\core\size.h>
#include <phi\rendering\renderer.h>
#include <phi\scenes\scene.h>

#include "form.h"
#include "defaultCameraController.h"
#include "library.h"
#include "inputManager.h"

#include <SDL/SDL.h>

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

    phi::scene* _scene;
    phi::pipeline* _pipeline;
    phi::renderer* _renderer;
    std::string _resourcesPath;

    library* _library;

    commandsManager* _commandsManager;
    inputManager* _inputManager;
    defaultCameraController* _defaultController;
private:
    void initScenesManager();
    void initScene();

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

    void setSceneRenderer(phi::renderer* value) { _renderer = value; }
    void setResourcesPath(std::string value) { _resourcesPath = value; }
};

#endif