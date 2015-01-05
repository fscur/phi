#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <SDL/SDL.h>
#include "scenesManager.h"
#include "Form.h"
#include "hud.h"
#include "button.h"
#include "label.h"
#include "slider.h"
#include "ray.h"
#include "size.h"

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

    phi::hud* _hud;
    phi::label* _labelFps;
    phi::label* _labelObjects;
    phi::label* _labelDt;
    phi::label* _labelInputCost;
    phi::label* _labelUpdateCost;
    phi::label* _labelRenderCost;
    phi::slider* _slider1;

private:
	void initScenesManager();
    void initScene();
    void initUI();
public:
    screen();
    ~screen(void);

    void initialize(std::string applicationPath) override;
    void onResize(SDL_Event e) override;
    void onMouseDown(SDL_Event e) override;
    void onMouseMove(SDL_Event e) override;
    void onMouseUp(SDL_Event e) override;
    void onMouseWheel(SDL_Event e) override;
    void onKeyDown(SDL_Event e) override;
    void onKeyUp(SDL_Event e) override;
    void onClosing() override;
    void closeButtonClick(phi::mouseEventArgs e);
    void expandButtonClick(phi::mouseEventArgs e);
    void sliderValueChanged(phi::eventArgs e);

    void update() override;
    void render() override;
};

#endif