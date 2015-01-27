#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <SDL/SDL.h>
#include "scenesManager.h"
#include "Form.h"
#include "button.h"
#include "label.h"
#include "slider.h"
#include "textBox.h"
#include "ray.h"
#include "size.h"
#include "commandsManager.h"

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
    phi::textBox* _textBox1;
    phi::textBox* _textBox2;
	phi::pointLight* _blueLight;

	phi::commandsManager _commandsManager;
private:
	void initScenesManager();
    void initScene();
    void initUI();
public:
    screen();
    ~screen(void);

    void initialize(std::string applicationPath) override;
    void onResize(SDL_Event e) override;
    void onClosing() override;

	void onBeginInput() override;
    void onMouseDown(phi::mouseEventArgs e) override;
    void onMouseMove(phi::mouseEventArgs e) override;
    void onMouseUp(phi::mouseEventArgs e) override;
    void onMouseWheel(phi::mouseEventArgs e) override;
    void onKeyDown(phi::keyboardEventArgs e) override;
    void onKeyUp(phi::keyboardEventArgs e) override;
	void onEndInput() override;

    void closeButtonClick(phi::mouseEventArgs e);
    void expandButtonClick(phi::mouseEventArgs e);
    void sliderValueChanged(phi::eventArgs e);
    void textBox1TextChanged(phi::eventArgs e);
    void textBox2TextChanged(phi::eventArgs e);
    void hudControlGotFocus(phi::controlEventArgs e);
    void hudControlLostFocus(phi::controlEventArgs e);
    

    void update() override;
    void render() override;
};

#endif