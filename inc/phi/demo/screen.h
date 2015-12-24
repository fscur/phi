#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "phi/core/size.h"

#include "form.h"
#include "fpsCameraController.h"
#include "defaultCameraController.h"
#include "library.h"

#include <phi\scenes\sceneRenderer.h>

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
    phi::sceneRenderer* _sceneRenderer;
    std::string _resourcesPath;

    library* _library;

private:
    void initScene();

public:
    screen();
    ~screen(void);

    void onInitialize() override;
    void onResize(SDL_Event e) override;
    void onClosing() override;

    void update() override;
    void render() override;

    void setSceneRenderer(phi::sceneRenderer* value) { _sceneRenderer = value; }
    void setResourcesPath(std::string value) { _resourcesPath = value; }
};

#endif