#include "screen.h"
#include "box.h"
#include "plane.h"
#include "sphere.h"
#include "cone.h"
#include "bunny.h"
#include "colorAnimator.h"
#include "fsSceneRenderer.h"
#include "dsSceneRenderer.h"
#include "renderingSystem.h"
#include "uiRepository.h"
#include "uiSystem.h"
#include "renderingSystem.h"
#include "renderingCommunicationBuffer.h"
#include "testSceneRenderer.h"

float a;
float angle;

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
    s->getActiveCamera()->setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
    s->getActiveCamera()->setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    s->setAmbientLightColor(phi::color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

    //phi::sceneObject* tank = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("tank.model"));
    //s->add(tank);
    /*
    phi::sceneObject* lamp = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("lamp.model"));
    auto lampSize = lamp->getSize();
    lamp->setPosition(glm::vec3(-0.3, 0.01, -0.5));
    s->add(lamp);

    phi::sceneObject* lamp1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("lamp.model"));
    lampSize = lamp1->getSize();
    lamp1->setPosition(glm::vec3(3.6, 0.01, -2.5));
    //s->add(lamp1);

    phi::sceneObject* table = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("table.model"));
    s->add(table);

    phi::sceneObject* carpet = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("carpet_1.model"));
    carpet->setPosition(glm::vec3(0.0, 0.03, 0.0));
    carpet->setSize(phi::size<float>(2.0, 1.0, 2.0));
    s->add(carpet);

    phi::sceneObject* brown_leather = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("test3.model"));
    brown_leather->setPosition(glm::vec3(-1.0, 0.0, 0.0));
    brown_leather->yaw(0.3);
    s->add(brown_leather);

    phi::sceneObject* brown_leather0 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("test3.model"));
    brown_leather0->setPosition(glm::vec3(1.0, 0.0, 0.0));
    brown_leather0->yaw(-0.3);
    s->add(brown_leather0);

    */
    /*
    for (int i = -2; i < 2; i++)
    {
    for (int j = -2; j < 2; j++)
    {
    auto w = 2.5;
    auto hw = 1.25;

    phi::sceneObject* floor = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("floor.model"));
    glm::vec3 pos(i * w + hw, 0.00, j *w+ hw);
    floor->setPosition(pos);
    floor->setSize(phi::size<float>(w, 0.01, w));
    s->add(floor);
    }
    }

    /*
    phi::plane* ceiling = new phi::plane(glm::vec3(0.0f, 1.0f, 0.0f), 10.0f, 10.0f, _resourcesLoader.getDefaultMaterial());
    ceiling->pitch(phi::PI);
    ceiling->setPosition(glm::vec3(0.0, 5.0, 0.0));
    s->add(ceiling);

    phi::plane* backWall = new phi::plane(glm::vec3(0.0f, 1.0f, 0.0f), 10.0f, 5.0f, _resourcesLoader.getDefaultMaterial());
    backWall->pitch(phi::PI_OVER_2);
    backWall->setPosition(glm::vec3(0.0, 2.5, -5.0));
    s->add(backWall);

    phi::plane* frontWall = new phi::plane(glm::vec3(0.0f, 1.0f, 0.0f), 10.0f, 5.0f, _resourcesLoader.getDefaultMaterial());
    frontWall->pitch(-phi::PI_OVER_2);
    frontWall->setPosition(glm::vec3(0.0, 2.5, 5.0));
    s->add(frontWall);

    phi::plane* leftWall = new phi::plane(glm::vec3(0.0f, 1.0f, 0.0f), 5.0f, 10.0f, _resourcesLoader.getDefaultMaterial());
    leftWall->roll(-phi::PI_OVER_2);
    leftWall->setPosition(glm::vec3(-5.0, 2.5, 0.0));
    s->add(leftWall);

    phi::plane* rightWall = new phi::plane(glm::vec3(0.0f, 1.0f, 0.0f), 5.0f, 10.0f, _resourcesLoader.getDefaultMaterial());
    rightWall->roll(phi::PI_OVER_2);
    rightWall->setPosition(glm::vec3(5.0, 2.5, 0.0));
    s->add(rightWall);*/


    phi::plane* floor = new phi::plane(glm::vec3(0.0f, 1.0f, 0.0f), 1000.0f, 1000.0f, _resourcesLoader.getDefaultMaterial());
    s->add(floor);

    phi::sceneObject* cube = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("cube.model"));
    cube->setPosition(glm::vec3(0.0, 0.5, 0.0));
    s->add(cube);

    /*

    phi::plane* ceiling = new phi::plane(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f, _resourcesLoader.getDefaultMaterial());
    ceiling->pitch(phi::PI);
    ceiling->setPosition(glm::vec3(-2.0, 0.5, -0.5));
    //s->add(ceiling);




    phi::sceneObject* cube1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("cube.model"));
    //cube1->setSize(phi::size<float>(2.0f, 2.0f, 0.0f));
    cube1->setPosition(glm::vec3(0.0, 0.5, 0.0));
    s->add(cube1);


    phi::sphere* sphere = new phi::sphere(0.5, 32, 32, _resourcesLoader.getDefaultMaterial());
    sphere->setPosition(glm::vec3(2.0, 0.5, 0.0));
    s->add(sphere);
    */

    glm::vec3 dirLightPos = glm::vec3(-3.0f, 3.0f, 3.0f);
    phi::directionalLight* dirLight = new phi::directionalLight(glm::vec3(), phi::color::fromRGBA(1.0, 0.9, 0.6, 1.0), 0.8f, -dirLightPos);
    dirLight->setPosition(dirLightPos);
    //s->add(dirLight);
    //

    dirLightPos = glm::vec3(-5.0f, 1.0f, -3.0f);
    phi::directionalLight* dirLight1 = new phi::directionalLight(glm::vec3(), phi::color::fromRGBA(1.0, 0.9, 0.6, 1.0), 1.0f, -dirLightPos);
    dirLight1->setPosition(dirLightPos);
    //s->add(dirLight1);

    phi::pointLight* pointLight0 = new phi::pointLight(glm::vec3(-0.3, 1.7, -0.505), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 1.5f, 10.0f);
    //s->add(pointLight0);

    phi::pointLight* pointLight1 = new phi::pointLight(glm::vec3(3.6, 1.7, -2.5), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 1.5f, 10.0f);
    //s->add(pointLight1);

    //phi::spotLight* spotLight0 = new phi::spotLight(glm::vec3(-3.0f, 3.0f, -3.0f), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 2.0f, 10.0f, glm::vec3(1.0f, -1.0f, 1.0f), 0.8);
    ///s->add(spotLight0);

    phi::scenesManager::get()->addScene(s);
    phi::scenesManager::get()->loadScene(0);
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
    closeButton->setX(1024 - 24);
    closeButton->setY(0);
    closeButton->getClick()->bind<screen, &screen::closeButtonClick>(this);
    phi::uiSystem::get()->addControl(closeButton);

    phi::button* buttonA = new phi::button(getSize());
    buttonA->setText("A");
    buttonA->setBackgroundColor(phi::color::red);
    buttonA->setForegroundColor(phi::color::white);
    buttonA->setSize(phi::size<GLuint>(64, 64));
    buttonA->setZIndex(20.0f);
    buttonA->setX(0);
    buttonA->setY(0);
    buttonA->getClick()->bind<screen, &screen::expandButtonClick>(this);

    _slider1 = new phi::slider(getSize());
    _slider1->setTrackColor(phi::color::gray);
    _slider1->setSliderColor(phi::color::blue);
    _slider1->setSize(phi::size<GLuint>(150, 32));
    _slider1->setZIndex(30.0f);
    _slider1->setX(5);
    _slider1->setY(148);
    _slider1->setMinValue(0.0f);
    _slider1->setMaxValue(1.0f);
    _slider1->setValue(0.0f);
    _slider1->setStepValue(0.05f);
    _slider1->getValueChanged()->bind<screen, &screen::slider1ValueChanged>(this);

    _slider2 = new phi::slider(getSize());
    _slider2->setTrackColor(phi::color::gray);
    _slider2->setSliderColor(phi::color::blue);
    _slider2->setSize(phi::size<GLuint>(150, 32));
    _slider2->setZIndex(30.0f);
    _slider2->setX(5);
    _slider2->setY(185);
    _slider2->setMinValue(0.0f);
    _slider2->setMaxValue(10.0f);
    _slider2->setValue(0.0f);
    _slider2->setStepValue(0.1f);
    _slider2->getValueChanged()->bind<screen, &screen::slider2ValueChanged>(this);

    _slider3 = new phi::slider(getSize());
    _slider3->setTrackColor(phi::color::gray);
    _slider3->setSliderColor(phi::color::blue);
    _slider3->setSize(phi::size<GLuint>(150, 32));
    _slider3->setZIndex(30.0f);
    _slider3->setX(5);
    _slider3->setY(223);
    _slider3->setMinValue(0.0f);
    _slider3->setMaxValue(10.0f);
    _slider3->setValue(0.0f);
    _slider3->setStepValue(0.1f);
    _slider3->getValueChanged()->bind<screen, &screen::slider3ValueChanged>(this);

    _slider4 = new phi::slider(getSize());
    _slider4->setTrackColor(phi::color::gray);
    _slider4->setSliderColor(phi::color::blue);
    _slider4->setSize(phi::size<GLuint>(150, 32));
    _slider4->setZIndex(30.0f);
    _slider4->setX(5);
    _slider4->setY(260);
    _slider4->setMinValue(0.0f);
    _slider4->setMaxValue(1.0f);
    _slider4->setValue(0.0f);
    _slider4->setStepValue(0.01f);
    _slider4->getValueChanged()->bind<screen, &screen::slider4ValueChanged>(this);

    //phi::textBox* textBoxA = new phi::textBox(getSize());
    //textBoxA->setText("Text");
    //textBoxA->setBackgroundColor(phi::color::gray);
    //textBoxA->setForegroundColor(phi::color::black);
    //textBoxA->setSize(phi::size<GLuint>(128, 32));
    //textBoxA->setZIndex(40.0f);
    //textBoxA->setX(132);
    //textBoxA->setY(180-48);

    /*phi::carouselList* carouselListA = new phi::carouselList(getSize());
    carouselListA->setBackgroundColor(phi::color::fromRGBA(0.3f, 0.3, 0.3f, 0.3f));
    carouselListA->setSize(phi::size<unsigned int>(getSize().width - 10, 150));
    carouselListA->setX(5);
    carouselListA->setY(getSize().height - 155);
    carouselListA->setZIndex(0);


    phi::carouselItem* carouselItemA = new phi::carouselItem(getSize());
    carouselItemA->setTexture(_texturesRepository->getResource<phi::texture>("diffuse.bmp"));
    phi::carouselItem* carouselItemB = new phi::carouselItem(getSize());
    carouselItemB->setTexture(_texturesRepository->getResource<phi::texture>("normal.bmp"));
    phi::carouselItem* carouselItemC = new phi::carouselItem(getSize());
    carouselItemC->setTexture(_texturesRepository->getResource<phi::texture>("specular.bmp"));
    phi::carouselItem* carouselItemD = new phi::carouselItem(getSize());
    carouselItemD->setTexture(_texturesRepository->getResource<phi::texture>("smile.bmp"));
    phi::carouselItem* carouselItemE = new phi::carouselItem(getSize());
    carouselItemE->setTexture(_texturesRepository->getResource<phi::texture>("floor.bmp"));
    phi::carouselItem* carouselItemF = new phi::carouselItem(getSize());
    phi::carouselItem* carouselItemG = new phi::carouselItem(getSize());
    phi::carouselItem* carouselItemH = new phi::carouselItem(getSize());
    phi::carouselItem* carouselItemI = new phi::carouselItem(getSize());
    phi::carouselItem* carouselItemJ = new phi::carouselItem(getSize());
    phi::carouselItem* carouselItemK = new phi::carouselItem(getSize());
    phi::carouselItem* carouselItemL = new phi::carouselItem(getSize());
    phi::carouselItem* carouselItemM = new phi::carouselItem(getSize());
    phi::carouselItem* carouselItemN = new phi::carouselItem(getSize());
    phi::carouselItem* carouselItemO = new phi::carouselItem(getSize());
    carouselListA->addCarouselItem(carouselItemA);
    carouselListA->addCarouselItem(carouselItemB);
    carouselListA->addCarouselItem(carouselItemC);
    carouselListA->addCarouselItem(carouselItemD);
    carouselListA->addCarouselItem(carouselItemE);
    carouselListA->addCarouselItem(carouselItemF);
    carouselListA->addCarouselItem(carouselItemG);
    carouselListA->addCarouselItem(carouselItemH);
    carouselListA->addCarouselItem(carouselItemI);
    carouselListA->addCarouselItem(carouselItemJ);
    carouselListA->addCarouselItem(carouselItemK);
    carouselListA->addCarouselItem(carouselItemL);
    carouselListA->addCarouselItem(carouselItemM);
    carouselListA->addCarouselItem(carouselItemN);
    carouselListA->addCarouselItem(carouselItemO);*/

    phi::uiSystem::get()->addControl(buttonA);
    //phi::uiSystem::get()->addControl(_slider1);
    //phi::uiSystem::get()->addControl(_slider2);
    //phi::uiSystem::get()->addControl(_slider3);
    //phi::uiSystem::get()->addControl(_slider4);
    //phi::uiSystem::get()->addControl(carouselListA);

    phi::uiSystem::get()->resize(getSize());

    phi::uiSystem::get()->getControlGotFocus()->bind<screen, &screen::hudControlGotFocus>(this);
    phi::uiSystem::get()->getControlLostFocus()->bind<screen, &screen::hudControlLostFocus>(this);
}

void screen::onInitialize()
{
    setTitle("Teste");
    //setSize(phi::size<unsigned int>(1800, 1000));
    //setSize(phi::size<unsigned int>(1024, 768));
    centerScreen();
    _resourcesLoader.load();
    _modelsRepository = _resourcesLoader.getModels();
    _texturesRepository = _resourcesLoader.getTextures();
    initScenesManager();
    initScene();
    initUI();
}

float t = 0.0f;

void screen::update()
{
    a+= 0.01;
    if (a > 2 * phi::PI)
        a -=2 * phi::PI;

    if (t > 24.0f)
        t -=24.0f;

    t += 0.01f;

    phi::scenesManager::get()->update();
    phi::colorAnimator::update();
    phi::uiSystem::get()->update();
    _commandsManager.update();

    /*_labelFps->setText("FPS: " + std::to_string(getFps()));

    _labelDt->setText("TotalTime/Frame: " + std::to_string(getDt()));
    _labelInputCost->setText("InputCost: " + std::to_string(getInputCost()));
    _labelUpdateCost->setText("UpdateCost: " + std::to_string(getUpdateCost()));
    _labelRenderCost->setText("RenderCost: " + std::to_string(getRenderCost()));*/

    //int visibleObjects = scenesManager::Get()->GetScene()->GetVisibleObjectsCount();
    //int allObjects = scenesManager::Get()->GetScene()->GetAllObjectsCount();
    //_labelObjects->setText(to_string(visibleObjects) + "/" + to_string(allObjects));

    //phi::pointLight* p = phi::scenesManager::get()->getScene()->getPointLight(0);
    //p->setPosition(glm::vec3(glm::cos(a) * 3.5f, 1.5f, glm::sin(a) * 3.5f));

    /*phi::spotLight* spotLight = phi::scenesManager::get()->getScene()->getSpotLight(0);
    auto dir = spotLight->getDirection();
    glm::vec3 spotLightPos = glm::vec3(glm::cos(a) * 4.0f, glm::abs(glm::sin(a) * 4.0f), dir.z);
    spotLight->setDirection(-spotLightPos);*/

    //phi::pointLight* pointLight = phi::scenesManager::get()->getScene()->getPointLight(0);
    //glm::vec3 pos = pointLight->getPosition();
    //glm::vec3 t = glm::vec3(-glm::cos(a) * 3.0f, pos.y, glm::sin(a) * 3.0f);
    //pointLight->setPosition(t);

    //phi::testSceneRenderer* renderer = dynamic_cast<phi::testSceneRenderer*>(phi::scenesManager::get()->getSceneRenderer());
    //renderer->setDayTime(7);
    //renderer->setSunOrientation(45);
}

void screen::render()
{
    phi::renderingSystem::defaultFrameBuffer->bind();
    phi::renderingSystem::defaultFrameBuffer->clear();

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

void screen::onMouseDown(phi::mouseEventArgs* e)
{
    if (_commandsManager.onMouseDown(e))
        return;
}

void screen::onMouseMove(phi::mouseEventArgs* e) 
{
    if (_commandsManager.onMouseMove(e))
        return;
}

void screen::onMouseUp(phi::mouseEventArgs* e)
{
    if (_commandsManager.onMouseUp(e))
        return;
}

void screen::onMouseWheel(phi::mouseEventArgs* e)
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

void screen::expandButtonClick(phi::mouseEventArgs* e)
{
    _sceneRenderer->setSSAOActive(_ssao);
    _ssao = !_ssao;
}

void screen::slider1ValueChanged(phi::eventArgs e)
{
    _sceneRenderer->setSSAOBias(_slider1->getValue());
    phi::spotLight* spotLight = phi::scenesManager::get()->getScene()->getSpotLight(0);
    spotLight->setCutoff(_slider1->getValue());
}

void screen::slider2ValueChanged(phi::eventArgs e)
{
    _sceneRenderer->setSSAOScale(_slider2->getValue());
}

void screen::slider3ValueChanged(phi::eventArgs e)
{
    _sceneRenderer->setSSAOIntensity(_slider3->getValue());
}

void screen::slider4ValueChanged(phi::eventArgs e)
{
    _sceneRenderer->setSSAOSampleRadius(_slider4->getValue());
}

void screen::closeButtonClick(phi::mouseEventArgs* e)
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