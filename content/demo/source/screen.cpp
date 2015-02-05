#include "screen.h"
#include "box.h"
#include "plane.h"
#include "sphere.h"
#include "cone.h"
#include "colorAnimator.h"
#include "fsSceneRenderer.h"
#include "dsSceneRenderer.h"
#include "renderingSystem.h"
#include "uiRepository.h"
#include "uiSystem.h"
#include "renderingSystem.h"
#include "renderingCommunicationBuffer.h"

float a;

screen::screen() : form()
{
    _temp = 0.0f;
    _isMouseDown = false;
    _rotating = false;
    _shadowMap = false;
    _translationSpeed = 1.0f;
    _rotationSpeed = 0.01f;
}

screen::~screen()
{
    phi::scenesManager::get()->release();
}

void screen::initScenesManager()
{
    phi::scenesManagerInfo info;
    info.applicationPath = getApplicationPath();
    info.size = getSize();

    phi::scenesManager::get()->init(info);
}

void screen::initScene()
{
    phi::scene* s = new phi::scene();
    s->setSize(getSize());
    s->getActiveCamera()->setPosition(glm::vec3(0.0f, 1.0f, 5.0f));
    s->getActiveCamera()->setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    s->setAmbientLightColor(phi::color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

    phi::box* box1 = new phi::box(glm::vec3(), phi::size<float>(1.0f, 1.0f, 1.0f), phi::renderingSystem::repository->getResource<phi::material>("bricks"));
    box1->setPosition(glm::vec3(0.0f, 0.5f, 0.0f));
    box1->setSelected(true);
    s->add(box1);

    //phi::sphere* sphere = new phi::sphere(glm::vec3(), 0.5f, 16, 32, phi::renderingSystem::repository->getResource<phi::material>("red"));
    //sphere->setPosition(glm::vec3(2.0f, 0.5f, 0.0f));
    //s->add(sphere);

    phi::plane* floor = new phi::plane(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), phi::size<float>(17.0f, 0.5f, 17.0f), phi::renderingSystem::repository->getResource<phi::material>("white"));
    s->add(floor);

    phi::directionalLight* dirLight = new phi::directionalLight(glm::vec3(0.0f, 2.0f, 2.0f), phi::color::white, 1.0f, glm::vec3(1.0f, -1.0f, -1.0f));
    s->add(dirLight);

    //phi::pointLight* pLight = new phi::pointLight(glm::vec3(0.0f, 5.0f, 0.0f), phi::color::blue, 10.0f, phi::attenuation());
    //s->add(pLight);

    //phi::spotLight* sLight = new phi::spotLight(glm::vec3(0.0f, 5.0f, 0.0f), phi::color::orange, 100.0f, phi::attenuation(), glm::vec3(0.0f, -1.0f, 0.0f), 0.8f);
    //s->add(sLight);

    phi::scenesManager::get()->addScene(s);
    phi::scenesManager::get()->loadScene(0);

    phi::camera* debugCamera = new phi::camera(0.1f, 1000.0f, 800.0f / 600.0f, phi::PI_OVER_4);
    debugCamera->setPosition(glm::vec3(50.0f, 0.0f, 0.0f));
    debugCamera->setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    s->add(debugCamera);
}

void screen::initUI()
{
    phi::uiSystemInfo info = phi::uiSystemInfo();
    info.applicationPath = getApplicationPath();
    info.size = getSize();
    phi::uiSystem::get()->init(info);

    phi::color labelBackground = phi::color::transparent;
    phi::color labelForeground = phi::color::white;

    phi::button* closeButton = new phi::button(getSize());
    closeButton->setText("X");
    closeButton->setBackgroundColor(phi::color::red);
    closeButton->setForegroundColor(phi::color::white);
    closeButton->setSize(phi::size<GLuint>(24, 24));
    closeButton->setX(1024 - 28);
    closeButton->setY(4);
    closeButton->getClick()->bind<screen, &screen::closeButtonClick>(this);
    phi::uiSystem::get()->addControl(closeButton);

    phi::button* expandButton = new phi::button(getSize());
    expandButton->setText("Texto\nBonito");
    expandButton->setBackgroundColor(phi::color::blue);
    expandButton->setForegroundColor(phi::color::white);
    expandButton->setSize(phi::size<GLuint>(48, 48));
    expandButton->setX(10);
    expandButton->setY(10);
    expandButton->getClick()->bind<screen, &screen::expandButtonClick>(this);
    phi::uiSystem::get()->addControl(expandButton);

    _textBox1 = new phi::textBox(getSize());
    _textBox1->setText("0123456789A0123456789B0123456789C0123456789D0123456789");
    _textBox1->setBackgroundColor(phi::color::white);
    _textBox1->setForegroundColor(phi::color::black);
    _textBox1->setSize(phi::size<GLuint>(200, 30));
    _textBox1->setX(600);
    _textBox1->setY(80);
    _textBox1->setFont(phi::uiRepository::repository->getResource<phi::font>("Consola_14"));
    _textBox1->getTextChanged()->bind<screen, &screen::textBox1TextChanged>(this);
    phi::uiSystem::get()->addControl(_textBox1);

    _textBox2 = new phi::textBox(getSize());
    _textBox2->setText("0123456789A0123456789B0123456789C0123456789D0123456789");
    _textBox2->setBackgroundColor(phi::color::white);
    _textBox2->setForegroundColor(phi::color::black);
    _textBox2->setSize(phi::size<GLuint>(200, 30));
    _textBox2->setX(600);
    _textBox2->setY(120);
    _textBox2->setFont(phi::uiRepository::repository->getResource<phi::font>("Consola_14"));
    _textBox2->getTextChanged()->bind<screen, &screen::textBox2TextChanged>(this);
    phi::uiSystem::get()->addControl(_textBox2);

    _labelFps = new phi::label(getSize());
    _labelFps->setX(68);
    _labelFps->setY(10);
    _labelFps->setSize(phi::size<GLuint>(150, 20));
    _labelFps->setForegroundColor(labelForeground);
    _labelFps->setBackgroundColor(labelBackground);
    _labelFps->setFont(phi::uiRepository::repository->getResource<phi::font>("Consola_24"));
    phi::uiSystem::get()->addControl(_labelFps);

    _labelObjects= new phi::label(getSize());
    _labelObjects->setX(68);
    _labelObjects->setY(28);
    _labelObjects->setSize(phi::size<GLuint>(150, 20));
    _labelObjects->setForegroundColor(labelForeground);
    _labelObjects->setBackgroundColor(labelBackground);
    phi::uiSystem::get()->addControl(_labelObjects);

    _labelDt = new phi::label(getSize());
    _labelDt->setX(220);
    _labelDt->setY(10);
    _labelDt->setSize(phi::size<GLuint>(300, 20));
    _labelDt->setForegroundColor(labelBackground);
    _labelDt->setBackgroundColor(labelBackground);
    phi::uiSystem::get()->addControl(_labelDt);

    _labelInputCost = new phi::label(getSize());
    _labelInputCost->setX(220);
    _labelInputCost->setY(28);
    _labelInputCost->setSize(phi::size<GLuint>(300, 20));
    _labelInputCost->setForegroundColor(labelForeground);
    _labelInputCost->setBackgroundColor(labelBackground);
    phi::uiSystem::get()->addControl(_labelInputCost);

    _labelUpdateCost = new phi::label(getSize());
    _labelUpdateCost->setX(220);
    _labelUpdateCost->setY(46);
    _labelUpdateCost->setSize(phi::size<GLuint>(300, 20));
    _labelUpdateCost->setForegroundColor(labelForeground);
    _labelUpdateCost->setBackgroundColor(labelBackground);
    phi::uiSystem::get()->addControl(_labelUpdateCost);

    _labelRenderCost = new phi::label(getSize());
    _labelRenderCost->setX(220);
    _labelRenderCost->setY(64);
    _labelRenderCost->setSize(phi::size<GLuint>(300, 20));
    _labelRenderCost->setForegroundColor(labelForeground);
    _labelRenderCost->setBackgroundColor(labelBackground);
    phi::uiSystem::get()->addControl(_labelRenderCost);

    _slider1 = new phi::slider(getSize());
    _slider1->setX(20);
    _slider1->setY(200);
    _slider1->setSize(phi::size<GLuint>(500, 30));
    _slider1->setSliderColor(phi::color::gray);
    _slider1->setTrackColor(phi::color::white);
    _slider1->setMinValue(0);
    _slider1->setMaxValue(200);
    _slider1->setStepValue(1.0f);
    _slider1->getValueChanged()->bind<screen, &screen::sliderValueChanged>(this);
    phi::uiSystem::get()->addControl(_slider1);

    phi::uiSystem::get()->resize(getSize());

    phi::uiSystem::get()->getControlGotFocus()->bind<screen, &screen::hudControlGotFocus>(this);
    phi::uiSystem::get()->getControlLostFocus()->bind<screen, &screen::hudControlLostFocus>(this);
}

void screen::onInitialize()
{
    setTitle("Teste");
    setSize(phi::size<unsigned int>(1024, 768));
    centerScreen();

    initScenesManager();
    initScene();
    initUI();
}

float angle;

void screen::update()
{
    a+= 0.00001;
    if (a > 2 * phi::PI)
        a -=2 * phi::PI;

    phi::scenesManager::get()->update();
    phi::colorAnimator::update();
    _commandsManager.update();

    _labelFps->setText("FPS: " + std::to_string(getFps()));
    //_labelDt->setText("TotalTime/Frame: " + std::to_string(getDt()));
    //_labelInputCost->setText("InputCost: " + std::to_string(getInputCost()));
    //_labelUpdateCost->setText("UpdateCost: " + std::to_string(getUpdateCost()));
    //_labelRenderCost->setText("RenderCost: " + std::to_string(getRenderCost()));

    //int visibleObjects = scenesManager::Get()->GetScene()->GetVisibleObjectsCount();
    //int allObjects = scenesManager::Get()->GetScene()->GetAllObjectsCount();
    //_labelObjects->setText(to_string(visibleObjects) + "/" + to_string(allObjects));
    /*
    phi::pointLight* p = phi::scenesManager::get()->getScene()->getPointLight(0);
    p->setPosition(glm::vec3(glm::cos(a) * 5.0f, 0.5f, glm::sin(a) * 3.5f));

    phi::spotLight* s = phi::scenesManager::get()->getScene()->getSpotLight(0);
    auto dir = s->getDirection();
    s->setDirection(glm::vec3(glm::cos(a) * 3.5f, -abs(glm::sin(a) * 3.5f), dir.z));*/

    /*phi::box* b0 = dynamic_cast<phi::box*>(phi::scenesManager::get()->getScene()->getSceneObject(0));
    glm::vec3 pos0 = phi::mathUtils::rotateAboutAxis(b0->getPosition(), glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.001);
    b0->setPosition(pos0);

    phi::box* b1 = dynamic_cast<phi::box*>(phi::scenesManager::get()->getScene()->getSceneObject(1));
    glm::vec3 pos1 = phi::mathUtils::rotateAboutAxis(b1->getPosition(), glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.001);
    b1->setPosition(pos1);*/

    phi::scene* scene = phi::scenesManager::get()->getScene();
    phi::camera* activeCamera = scene->getCamera(0);
    phi::camera* camera = scene->getCamera(1);
    glm::vec3 camPos = activeCamera->getPosition();
    camera->setPosition(glm::vec3(50.0f, 0.0f, camPos.z));

    //float dist = 3.0f;
    //activeCamera->setPosition(glm::vec3(glm::cos(a) * dist, 2.0f, glm::sin(a) * dist));
    //activeCamera->setTarget(glm::normalize(-glm::vec3(glm::cos(a) * dist, 1.0f, glm::sin(a) * dist)));
    //activeCamera->update();
}

void screen::render()
{
    phi::scenesManager::get()->render();
    phi::uiSystem::get()->render();
    phi::renderingCommunicationBuffer::update();
    /*
    phi::renderingSystem::mainRenderTarget->bind();
    GLuint w = getSize().width / 2;
    GLuint h = getSize().height;

    phi::renderingSystem::mainRenderTarget->setViewport(w, 0, phi::size<GLuint>(w, h));

    phi::scene* scene = phi::scenesManager::get()->getScene();
    phi::camera* camera = scene->getCamera(1);

    glm::mat4 proj = camera->getPerspProjMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((const GLfloat*)&proj[0]);

    glMatrixMode(GL_MODELVIEW);
    glm::mat4 MV = camera->getViewMatrix();
    glLoadMatrixf((const GLfloat*)&MV[0]);

    for (GLuint i = 0; i < scene->getAllObjectsCount(); i++)
    //if (_cameras[1]->GetFrustum().IsInside(_visibleObjects[i]))
    scene->getSceneObject(i)->debugRender();

    phi::camera* activeCamera = scene->getCamera(0);
    activeCamera->debugRender();

    /*unsigned int count = _allObjects.size();

    frustum frustum = _cameras[1]->GetFrustum();

    for (unsigned int i = 0; i < count; i++)
    {
    SceneObject* sceneObject = _allObjects[i];
    if (frustum.IsInside(sceneObject))
    sceneObject->DebugRender();
    }*/

    /*unsigned int count = _cameras.size();

    for (unsigned int i = 0; i < count; i++)
    if (_activeCamera != _cameras[i])
    _cameras[i]->DebugRender();*/
}

void screen::onResize(SDL_Event e)
{
    phi::size<GLuint> sz = phi::size<GLuint>(e.window.data1, e.window.data2);
    setSize(sz);
    phi::scenesManager::get()->resize(sz);
    phi::uiSystem::get()->resize(sz);
}

void screen::onBeginInput()
{
    _commandsManager.onBeginInput(getSize());
}

void screen::onMouseDown(phi::mouseEventArgs e)
{	
    if (_commandsManager.onMouseDown(e))
        return;
}

void screen::onMouseMove(phi::mouseEventArgs e) 
{
    if (_commandsManager.onMouseMove(e))
        return;
}

void screen::onMouseUp(phi::mouseEventArgs e)
{
    if (_commandsManager.onMouseUp(e))
        return;
}

void screen::onMouseWheel(phi::mouseEventArgs e)
{
    if (_commandsManager.onMouseWheel(e))
        return;
}

void screen::onKeyDown(phi::keyboardEventArgs e)
{
    if (_commandsManager.onKeyDown(e))
        return;

    if (e.key == PHIK_ESCAPE)
        close();
}

void screen::onKeyUp(phi::keyboardEventArgs e)
{
    if (_commandsManager.onKeyUp(e))
        return;
}

void screen::onEndInput()
{
    _commandsManager.onEndInput();
}

void screen::onClosing()
{
    //TODO: MessageBox asking if the user really wants to close the window
}

void screen::expandButtonClick(phi::mouseEventArgs e)
{
    _textBox1->setText("0123456789A0123456789B0123456789C0123456789D0123456789");
    _textBox2->setText("0123456789A0123456789B0123456789C0123456789D0123456789");
}

void screen::sliderValueChanged(phi::eventArgs e)
{
    _labelFps->setY(_slider1->getValue());
}

void screen::closeButtonClick(phi::mouseEventArgs e)
{
    close();
}

void screen::textBox1TextChanged(phi::eventArgs e)
{
    int value = atoi(_textBox1->getText().c_str());
    _labelFps->setX(value);
}

void screen::textBox2TextChanged(phi::eventArgs e)
{
    int value = atoi(_textBox2->getText().c_str());
    _labelFps->setY(value);
}

void screen::hudControlGotFocus(phi::controlEventArgs e)
{
    SDL_StartTextInput();
}

void screen::hudControlLostFocus(phi::controlEventArgs e)
{
    SDL_StopTextInput();
}