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


    phi::sceneObject* chair1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("black_leather.obj"));
    chair1->setPosition(glm::vec3(-1.0f, 0.0f, 0.0f));
    s->add(chair1);
    */
    phi::sceneObject* lamp = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("lamp.model"));
    s->add(lamp);

    //phi::sceneObject* cube = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("cube.model"));
    //s->add(cube);

    phi::plane* plane0 = new phi::plane(glm::vec3(0.0f, 1.0f, 0.0f), 10.0f, 10.0f, _resourcesLoader.getDefaultMaterial());
    s->add(plane0);

    //phi::plane* plane1 = new phi::plane(glm::vec3(1.0f, 0.0f, 0.0f), 10.0f, 10.0f, _resourcesLoader.getDefaultMaterial());
    //s->add(plane1);

    phi::plane* plane2 = new phi::plane(glm::vec3(0.0f, 0.0f, 1.0f), 10.0f, 10.0f, _resourcesLoader.getDefaultMaterial());
    s->add(plane2);

    //phi::directionalLight* dirLight = new phi::directionalLight(glm::vec3(), phi::color::white, 1.0f, glm::vec3(-0.2f, -0.4f, -0.5f));
    //s->add(dirLight);

    phi::pointLight* pointLight0 = new phi::pointLight(glm::vec3(0.0f, 1.7f, 0.0f), phi::color::red, 0.5f, 3.0f);
    s->add(pointLight0);

    /*
    phi::spotLight* spotLight0 = new phi::spotLight(glm::vec3(0.0f, 1.7f, 0.0f), phi::color::orange, 2.0f, 3.0f, glm::vec3(0.0f, -1.0f, 0.0f), 0.6);
    s->add(spotLight0);*/

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

    //phi::slider* sliderA = new phi::slider(getSize());
    //sliderA->setTrackColor(phi::color::white);
    //sliderA->setSliderColor(phi::color::blue);
    //sliderA->setSize(phi::size<GLuint>(128, 32));
    //sliderA->setZIndex(30.0f);
    //sliderA->setX(100);
    //sliderA->setY(148);
    //sliderA->setMinValue(0.0f);
    //sliderA->setMaxValue(1.0f);
    //sliderA->setValue(0.5f);
    //sliderA->setStepValue(0.01f);

    //phi::textBox* textBoxA = new phi::textBox(getSize());
    //textBoxA->setText("Text");
    //textBoxA->setBackgroundColor(phi::color::gray);
    //textBoxA->setForegroundColor(phi::color::black);
    //textBoxA->setSize(phi::size<GLuint>(128, 32));
    //textBoxA->setZIndex(40.0f);
    //textBoxA->setX(132);
    //textBoxA->setY(180-48);

    phi::carouselList* carouselListA = new phi::carouselList(getSize());
    carouselListA->setBackgroundColor(phi::color::fromRGBA(0.3f, 0.3, 0.3f, 0.3f));
    carouselListA->setSize(phi::size<unsigned int>(getSize().width - 10, 150));
    carouselListA->setX(5);
    carouselListA->setY(getSize().height - 155);
    carouselListA->setZIndex(0);

    phi::carouselItem* carouselItemA = new phi::carouselItem(getSize());
    carouselItemA->setTexture(_resourcesRepo->getResource<phi::texture>("diffuse.bmp"));
    phi::carouselItem* carouselItemB = new phi::carouselItem(getSize());
    carouselItemB->setTexture(_resourcesRepo->getResource<phi::texture>("normal.bmp"));
    phi::carouselItem* carouselItemC = new phi::carouselItem(getSize());
    carouselItemC->setTexture(_resourcesRepo->getResource<phi::texture>("specular.bmp"));
    phi::carouselItem* carouselItemD = new phi::carouselItem(getSize());
    carouselItemD->setTexture(_resourcesRepo->getResource<phi::texture>("test.bmp"));
    phi::carouselItem* carouselItemE = new phi::carouselItem(getSize());
    carouselItemE->setTexture(_resourcesRepo->getResource<phi::texture>("close.bmp"));
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
    carouselListA->addCarouselItem(carouselItemO);

    phi::uiSystem::get()->addControl(buttonA);
    phi::uiSystem::get()->addControl(carouselListA);

    phi::uiSystem::get()->resize(getSize());

    phi::uiSystem::get()->getControlGotFocus()->bind<screen, &screen::hudControlGotFocus>(this);
    phi::uiSystem::get()->getControlLostFocus()->bind<screen, &screen::hudControlLostFocus>(this);
}

void screen::onInitialize()
{
    setTitle("Teste");
    setSize(phi::size<unsigned int>(1024, 768));
    centerScreen();

    _resourcesLoader.load();
    _modelsRepository = _resourcesLoader.getModels();

    initScenesManager();
    initScene();
    initUI();
}

void screen::update()
{
    a+= 0.01;
    if (a > 2 * phi::PI)
        a -=2 * phi::PI;

    phi::scenesManager::get()->update();
    phi::colorAnimator::update();
    phi::uiSystem::get()->update();
    _commandsManager.update();

    //_labelFps->setText("FPS: " + std::to_string(getFps()));

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

    /*  phi::pointLight* p = phi::scenesManager::get()->getScene()->getPointLight(0);
    glm::vec3 pos = p->getPosition();
    glm::vec3 t = glm::vec3(-glm::cos(a), 0.0f, glm::sin(a));
    p->setPosition(pos + t);*/


    /*phi::pointLight* p = phi::scenesManager::get()->getScene()->getPointLight(3);
    glm::vec3 pos = p->getPosition();

    phi::box* b0 = dynamic_cast<phi::box*>(phi::scenesManager::get()->getScene()->getSceneObject(0));
    glm::vec3 pos0 = glm::vec3(glm::cos(a) * 5.0f, 0.5f, glm::sin(a) * 5.0f);
    b0->setPosition(pos0);

    phi::sphere* b1 = dynamic_cast<phi::sphere*>(phi::scenesManager::get()->getScene()->getSceneObject(1));
    pos0 = glm::vec3(-glm::cos(a) * 2.0f, 0.5f, glm::sin(a) * 2.0f);
    b1->setPosition(pos0);*/

    //float dist = 3.0f;
    //activeCamera->setPosition(glm::vec3(glm::cos(a) * dist, 2.0f, glm::sin(a) * dist));
    //activeCamera->setTarget(glm::normalize(-glm::vec3(glm::cos(a) * dist, 1.0f, glm::sin(a) * dist)));
    //activeCamera->update();
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
    _textBox1->setText("0123456789A0123456789B0123456789C0123456789D0123456789");
    _textBox2->setText("0123456789A0123456789B0123456789C0123456789D0123456789");
}

void screen::sliderValueChanged(phi::eventArgs e)
{
    //phi::pointLight* p = phi::scenesManager::get()->getScene()->getPointLight(0);
    //p->setIntensity(_slider1->getValue());

    //phi::spotLight* p1 = phi::scenesManager::get()->getScene()->getSpotLight(0);
    //p1->setIntensity(_slider1->getValue());
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