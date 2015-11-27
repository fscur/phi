#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "phi/core/size.h"

#include "phi/rendering/ray.h"

#include "phi/scenes/scenesManager.h"

#include "phi/ui/button.h"
#include "phi/ui/toggleButton.h"
#include "phi/ui/label.h"
#include "phi/ui/slider.h"
#include "phi/ui/textBox.h"
#include "phi/ui/carouselList.h"
#include "phi/ui/carouselItem.h"
#include "phi/ui/translationControl.h"
#include "phi/ui/rotationControl.h"
#include "phi/ui/button3D.h"

#include "phi/demo/form.h"
#include "phi/demo/commandsManager.h"
#include "phi/demo/resourcesLoader.h"
#include "phi/demo/inputManager.h"
#include "phi/demo/fpsCameraController.h"
#include "phi/demo/defaultCameraController.h"

#include <SDL/SDL.h>

class screen :
    public form
{
private:
    phi::ray _ray;
    bool _isMouseDown;
    glm::vec2 _mouseDownPos;
    glm::vec2 _lastMousePos;
    bool _rotating;
    bool _shadowMap;

    float _translationSpeed;
    float _rotationSpeed;

    float _temp;

    phi::label* _labelFps;
    phi::label* _labelObjects;
    phi::label* _labelDt;
    phi::label* _labelInputCost;
    phi::label* _labelUpdateCost;
    phi::label* _labelRenderCost;
    phi::slider* _slider1;
    phi::slider* _slider2;
    phi::slider* _slider3;
    phi::slider* _slider4;
    phi::textBox* _textBox1;
    phi::textBox* _textBox2;
	phi::pointLight* _blueLight;

    phi::sceneRenderer* _sceneRenderer;
    
    std::string _resourcesPath;
	phi::resourcesLoader _resourcesLoader;

    phi::resourcesRepository* _modelsRepository;
    phi::resourcesRepository* _materialsRepository;
    phi::resourcesRepository* _texturesRepository;
    commandsManager* _commandsManager;
    inputManager* _inputManager;
    fpsCameraController* _fpsController;
    defaultCameraController* _defaultController;

    bool _ssao;

private:
	void initScenesManager();
    void initScene();
    void initUI();

public:
    screen();
    ~screen(void);

    void onInitialize() override;
    void onResize(SDL_Event e) override;
    void onClosing() override;

	void onBeginInput() override;
    void onMouseDown(phi::mouseEventArgs* e) override;
    void onMouseMove(phi::mouseEventArgs* e) override;
    void onMouseUp(phi::mouseEventArgs* e) override;
    void onMouseWheel(phi::mouseEventArgs* e) override;
    void onKeyDown(phi::keyboardEventArgs* e) override;
    void onKeyUp(phi::keyboardEventArgs* e) override;
	void onEndInput() override;

    void closeButtonClick(phi::mouseEventArgs* e);
    void hideSelectedButtonClick(phi::mouseEventArgs* e);
    void showAllButtonClick(phi::mouseEventArgs* e);
    void importSEButtonClick(phi::mouseEventArgs* e);
    void slider1ValueChanged(phi::eventArgs e);
    void slider2ValueChanged(phi::eventArgs e);
    void slider3ValueChanged(phi::eventArgs e);
    void slider4ValueChanged(phi::eventArgs e);
    void textBox1TextChanged(phi::eventArgs e);
    void textBox2TextChanged(phi::eventArgs e);
    void hudControlGotFocus(phi::controlEventArgs e);
    void hudControlLostFocus(phi::controlEventArgs e);
    void selectedSceneObjectChanged(phi::sceneObjectEventArgs e);
    void staticObjectsChanged(phi::eventArgs e);
    void carouselListItemClick(phi::mouseEventArgs* e);
    void translating(phi::translationEventArgs* e);
    void translationFinished(phi::translationEventArgs* e);
    void rotating(phi::rotationEventArgs* e);
    void rotationFinished(phi::rotationEventArgs e);
    void dragDropEnded(phi::dragDropEventArgs* e);

    void update() override;
    void render() override;

    void setSceneRenderer(phi::sceneRenderer* value) { _sceneRenderer = value; }
    void setResourcesPath(std::string value) { _resourcesPath = value; }
};

#endif