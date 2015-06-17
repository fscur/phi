#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <SDL/SDL.h>
#include "scenesManager.h"
#include "Form.h"
#include "button.h"
#include "label.h"
#include "slider.h"
#include "textBox.h"
#include "carouselList.h"
#include "carouselItem.h"
#include "translationControl.h"
#include "rotationControl.h"
#include "ray.h"
#include "size.h"
#include "commandsManager.h"
#include "resourcesLoader.h"
#include "testSceneRenderer.h"

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

	phi::commandsManager _commandsManager;
    phi::resourcesLoader _resourcesLoader;
    phi::resourcesRepository* _modelsRepository;
    phi::resourcesRepository* _texturesRepository;

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
    void onKeyDown(phi::keyboardEventArgs e) override;
    void onKeyUp(phi::keyboardEventArgs e) override;
	void onEndInput() override;

    void closeButtonClick(phi::mouseEventArgs* e);
    void hideSelectedButtonClick(phi::mouseEventArgs* e);
    void showAllButtonClick(phi::mouseEventArgs* e);
    void slider1ValueChanged(phi::eventArgs e);
    void slider2ValueChanged(phi::eventArgs e);
    void slider3ValueChanged(phi::eventArgs e);
    void slider4ValueChanged(phi::eventArgs e);
    void textBox1TextChanged(phi::eventArgs e);
    void textBox2TextChanged(phi::eventArgs e);
    void hudControlGotFocus(phi::controlEventArgs e);
    void hudControlLostFocus(phi::controlEventArgs e);
    void selectedSceneObjectChanged(phi::sceneObjectEventArgs e);

    void update() override;
    void render() override;

    void setSceneRenderer(phi::sceneRenderer* value) { _sceneRenderer = value; }
};

#endif