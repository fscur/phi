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
phi::translationControl* tc;
phi::rotationControl* rc;
float t = 0.0f;
phi::sceneObject* sphere;

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
    s->getActiveCamera()->setPosition(glm::vec3(1.0f, 3.0f, 12.0f));
    s->getActiveCamera()->setTarget(glm::vec3(0.0f, 0.5f, 0.0f));
    s->setAmbientLightColor(phi::color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));
    s->getSelectedSceneObjectChanged()->bind<screen, &screen::selectedSceneObjectChanged>(this);
    
    /* decoration */
    
    phi::sceneObject* flower = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("flower.model"));
    flower->setPosition(glm::vec3(2.0f, 0.8f, 0.0f));
    s->add(flower);

    phi::sceneObject* portrait = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("portrait.model"));
    portrait->setSize(phi::size<float>(2.0f, 2.0f, 1.0f));
    portrait->setPosition(glm::vec3(-4.98f, 2.0f, 0.0f));
    portrait->yaw(phi::PI_OVER_2);
    s->add(portrait);

    phi::sceneObject* floor_lamp = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("floor_lamp.model"));
    floor_lamp->setPosition(glm::vec3(-4.7f, 0.0f, -1.7f));
    s->add(floor_lamp);

    phi::pointLight* pointLight_lamp = new phi::pointLight(glm::vec3(-4.7, 1.0, -1.7), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.8f, 10.0f);
    //s->add(pointLight_lamp);

    phi::sceneObject* telephone = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("telephone.model"));
    telephone->setPosition(glm::vec3(4.7f, 0.76f, 0.5f));
    telephone->yaw(-phi::PI_OVER_2);
    s->add(telephone);

    phi::sceneObject* pair_vase = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("pair_vase.model"));
    pair_vase->setPosition(glm::vec3(4.7f, 0.76f, -0.05f));
    s->add(pair_vase);

    phi::sceneObject* flower_vase = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("flower_vase.model"));
    flower_vase->setPosition(glm::vec3(4.7f, 0.76f, -0.20f));
    flower_vase->yaw(-1.5);
    s->add(flower_vase);

    phi::sceneObject* rug = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("persian_rug.model"));
    rug->setPosition(glm::vec3(-3.0f, 0.0f, -0.5f));
    s->add(rug);

    phi::sceneObject* rug1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("persian_rug_1.model"));
    rug1->setSize(phi::size<float>(1.25f, 1.0f, 1.25f));
    rug1->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));
    s->add(rug1);

    phi::sceneObject* casket = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("casket.model"));
    casket->setPosition(glm::vec3(-1.0f, 0.0f, 1.5f));
    casket->yaw(phi::PI + phi::PI_OVER_2);
    s->add(casket);

    ///* living room */
    
    phi::sceneObject* sofa = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("sofa.model"));
    sofa->setPosition(glm::vec3(-3.1f, 0.0f, 1.4f));
    sofa->yaw(phi::PI);
    s->add(sofa);

    phi::sceneObject* chair0 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("black_chair.model"));
    chair0->setPosition(glm::vec3(-3.8, 0.0, -1.0));
    chair0->yaw(0.3);
    s->add(chair0);

    phi::sceneObject* chair1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("black_chair.model"));
    chair1->setPosition(glm::vec3(-2.3, 0.0, -1.0));
    chair1->yaw(-0.3);
    s->add(chair1);

    ///*
    //phi::sceneObject* cabinet = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("cabinet.model"));
    //cabinet->setPosition(glm::vec3(0.0, 0.0, 2.0));
    ////s->add(cabinet);
    //*/

    ///* dining */
    
    phi::sceneObject* cabinet = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("cabinet.model"));
    cabinet->setPosition(glm::vec3(4.7f, 0.0f, 0.0f));
    cabinet->yaw(phi::PI);
    s->add(cabinet);

    phi::sceneObject* table = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("table2.model"));
    table->setPosition(glm::vec3(2.0f, 0.0f, 0.0f));
    s->add(table);

    phi::sceneObject* table_chair0 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("table_chair.model"));
    table_chair0->setPosition(glm::vec3(1.5, 0.0, -0.5f));
    s->add(table_chair0);

    phi::sceneObject* table_chair1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("table_chair.model"));
    table_chair1->setPosition(glm::vec3(2.5, 0.0, -0.5));
    table_chair1->yaw(-0.2);
    s->add(table_chair1);
    
    phi::sceneObject* table_chair2 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("table_chair.model"));
    table_chair2->setPosition(glm::vec3(1.5, 0.0, 0.5));
    table_chair2->yaw(phi::PI-0.3);
    s->add(table_chair2);

    phi::sceneObject* table_chair3 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("table_chair.model"));
    table_chair3->setPosition(glm::vec3(2.5, 0.0, 0.5));
    table_chair3->yaw(phi::PI + 0.4);
    s->add(table_chair3);

    phi::sceneObject* table_chair4 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("table_chair.model"));
    table_chair4->setPosition(glm::vec3(3.0, 0.0, 0.1));
    table_chair4->yaw(phi::PI_OVER_2 + phi::PI + 0.1);
    s->add(table_chair4);

    phi::sceneObject* table_chair5 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("table_chair.model"));
    table_chair5->setPosition(glm::vec3(0.8, 0.0, 0.0));
    table_chair5->yaw(phi::PI_OVER_2 - 0.1);
    s->add(table_chair5);
    //
    //

    ///* slider light */
    //
    //phi::pointLight* pointLight = new phi::pointLight(glm::vec3(-8.0f, 2.0f, 0.0f), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 1.4f, 5.0f);
    //s->add(pointLight);

    //sphere = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("sphere.model"));
    //sphere->setPosition(glm::vec3(-8.0f, 2.0f, 0.0f));
    //sphere->setSize(phi::size<float>(0.4f, 0.4f, 0.4f));
    //s->add(sphere);

    ///* lights */
    
    phi::sceneObject* lamp0 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("ceiling_lamp.model"));
    lamp0->setPosition(glm::vec3(0.0, 3.5, 0.0));
    s->add(lamp0);

    phi::pointLight* pointLight0 = new phi::pointLight(glm::vec3(0.0, 1.8, 0.0), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, 10.0f);
    //s->add(pointLight0);

    phi::sceneObject* lamp1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("ceiling_lamp.model"));
    lamp1->setPosition(glm::vec3(2.5, 3.5, 0.0));
    s->add(lamp1);

    phi::pointLight* pointLight1 = new phi::pointLight(glm::vec3(2.5, 1.8, 0.0), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, 10.0f);
    //s->add(pointLight1);
    
    phi::sceneObject* lamp2 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("ceiling_lamp.model"));
    lamp2->setPosition(glm::vec3(-2.5, 3.5, 0.0));
    s->add(lamp2);

    phi::pointLight* pointLight2 = new phi::pointLight(glm::vec3(-2.5, 1.8, 0.0), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, 10.0f);
    //s->add(pointLight2);

    ///* room */
    //
    phi::sceneObject* floor = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("floor.model"));
    floor->setPosition(glm::vec3(0.0, 0.0, 0.0));
    s->add(floor);
    
    phi::sceneObject* ceiling = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("ceiling.model"));
    ceiling->setPosition(glm::vec3(0.0, 3.5, 0.0));
    s->add(ceiling);

    phi::sceneObject* left_wall = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("side_wall.model"));
    left_wall->setPosition(glm::vec3(-5.0, 0.0, 0.0));
    s->add(left_wall);

    phi::sceneObject* right_wall = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("side_wall.model"));
    right_wall->setPosition(glm::vec3(5.0, 0.0, 0.0));
    right_wall->yaw(phi::PI);
    s->add(right_wall);

    phi::sceneObject* back_wall = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("back_wall.model"));
    back_wall->setPosition(glm::vec3(0.0, 0.0, -2.0));
    s->add(back_wall);

    phi::sceneObject* front_wall = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("front_wall.model"));
    front_wall->setPosition(glm::vec3(0.0, 0.0, 2.0));
    //s->add(front_wall);

    
    auto pos = glm::vec3(2.0, 5.0, -3.0);
    phi::directionalLight* dirLight0 = new phi::directionalLight(pos, phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, -pos);
    s->add(dirLight0);

    pos = glm::vec3(2.0, 5.0, 3.0);
    phi::directionalLight* dirLight1 = new phi::directionalLight(pos, phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, -pos);
    s->add(dirLight1);

    pos = glm::vec3(-2.0, 5.0, 3.0);
    phi::directionalLight* dirLight2 = new phi::directionalLight(pos, phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, -pos);
    s->add(dirLight2);

    pos = glm::vec3(-2.0, 5.0, -3.0);
    phi::directionalLight* dirLight3 = new phi::directionalLight(pos, phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, -pos);
    s->add(dirLight3);

    pos = glm::vec3(2.0, 2.0, -3.0);
    phi::directionalLight* dirLight4 = new phi::directionalLight(pos, phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, -pos);
    s->add(dirLight4);

    pos = glm::vec3(2.0, 2.0, 3.0);
    phi::directionalLight* dirLight5 = new phi::directionalLight(pos, phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, -pos);
    s->add(dirLight5);

    pos = glm::vec3(-2.0, 2.0, 3.0);
    phi::directionalLight* dirLight6 = new phi::directionalLight(pos, phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, -pos);
    s->add(dirLight6);

    pos = glm::vec3(-2.0, 2.0, -3.0);
    phi::directionalLight* dirLight7 = new phi::directionalLight(pos, phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, -pos);
    s->add(dirLight7);
    
    ///auto dir = glm::vec3(1.0, -1.0, 0.0);
    //phi::spotLight* spotLight0 = new phi::spotLight(glm::vec3(-2.0f, 2.0f, 0.0f), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 1.0f, 5.0f, dir, 0.9f);
    //s->add(spotLight0);

    //phi::pointLight* pointLight = new phi::pointLight(glm::vec3(-2.0f, 2.0f, 0.0f), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 1.0f, 5.0f);
    //s->add(pointLight0);

    //phi::sceneObject* cube = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("cube.model"));
    //cube->setPosition(glm::vec3(0.0f, 0.5f, 0.0f));
    //cube->setOrientation(glm::vec3(0.0f, 1.0f, 0.0f));
    //s->add(cube);
    


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
    closeButton->setX(getSize().width - 24);
    closeButton->setY(0);
    closeButton->getClick()->bind<screen, &screen::closeButtonClick>(this);
    phi::uiSystem::get()->addControl(closeButton);

    phi::button* buttonA = new phi::button(getSize());
    buttonA->setText("Hide selected");
    buttonA->setBackgroundColor(phi::color::red);
    buttonA->setForegroundColor(phi::color::white);
    buttonA->setSize(phi::size<GLuint>(200, 30));
    buttonA->setZIndex(20.0f);
    buttonA->setX(10);
    buttonA->setY(10);
    buttonA->getClick()->bind<screen, &screen::hideSelectedButtonClick>(this);

    phi::button* buttonB = new phi::button(getSize());
    buttonB->setText("Show all");
    buttonB->setBackgroundColor(phi::color::red);
    buttonB->setForegroundColor(phi::color::white);
    buttonB->setSize(phi::size<GLuint>(200, 30));
    buttonB->setZIndex(20.0f);
    buttonB->setX(10);
    buttonB->setY(50);
    buttonB->getClick()->bind<screen, &screen::showAllButtonClick>(this);

    _slider1 = new phi::slider(getSize());
    _slider1->setTrackColor(phi::color::gray);
    _slider1->setSliderColor(phi::color::blue);
    _slider1->setSize(phi::size<GLuint>(500, 32));
    _slider1->setZIndex(30.0f);
    _slider1->setX(5);
    _slider1->setY(148);
    _slider1->setMinValue(0.0f);
    _slider1->setMaxValue(phi::PI * 4.0f);
    _slider1->setValue(0.0f);
    _slider1->setStepValue(0.01f);
    _slider1->getValueChanged()->bind<screen, &screen::slider1ValueChanged>(this);

    _slider2 = new phi::slider(getSize());
    _slider2->setTrackColor(phi::color::gray);
    _slider2->setSliderColor(phi::color::blue);
    _slider2->setSize(phi::size<GLuint>(500, 32));
    _slider2->setZIndex(30.0f);
    _slider2->setX(5);
    _slider2->setY(185);
    _slider2->setMaxValue(4.0f);
    _slider2->setMinValue(1.0f);
    _slider2->setValue(1.0f);
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

    auto camera = phi::scenesManager::get()->getScene()->getActiveCamera();

    tc = new phi::translationControl(getSize());
    tc->setCamera(phi::scenesManager::get()->getScene()->getActiveCamera());
    phi::uiSystem::get()->addControl(tc);

    rc = new phi::rotationControl(getSize());
    rc->setCamera(phi::scenesManager::get()->getScene()->getActiveCamera());
    phi::uiSystem::get()->addControl(rc);

    phi::uiSystem::get()->addControl(buttonA);
    phi::uiSystem::get()->addControl(buttonB);
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
    centerScreen();
    _resourcesLoader.load();
    _modelsRepository = _resourcesLoader.getModels();
    _texturesRepository = _resourcesLoader.getTextures();
    initScenesManager();
    initScene();
    initUI();
}

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
    phi::sceneRenderer* renderer = phi::scenesManager::get()->getSceneRenderer();
    renderer->resize(sz);
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

void screen::hideSelectedButtonClick(phi::mouseEventArgs* e)
{
    auto allObjects = phi::scenesManager::get()->getScene()->getAllObjects();
    for (auto i = 0; i < allObjects->size();i++)
    {
        if  (allObjects->at(i)->getSelected())
            allObjects->at(i)->setActive(false);
    }
}

void screen::showAllButtonClick(phi::mouseEventArgs* e)
{
    auto allObjects = phi::scenesManager::get()->getScene()->getAllObjects();
    for (auto i = 0; i < allObjects->size();i++)
    {
        allObjects->at(i)->setActive(true);
    }
}

void screen::slider1ValueChanged(phi::eventArgs e)
{
    /*_sceneRenderer->setSSAOBias(_slider1->getValue());
    phi::spotLight* spotLight = phi::scenesManager::get()->getScene()->getSpotLight(0);
    spotLight->setCutoff(_slider1->getValue());*/

    auto pointLight = phi::scenesManager::get()->getScene()->getPointLight(0);
    glm::vec3 pos = pointLight->getPosition();
    auto value =_slider1->getValue();
    glm::vec3 t = glm::vec3(-glm::cos(value) * 8.0f, pos.y, glm::sin(value) * 4.0f);
    pointLight->setPosition(t);

    
    sphere->setPosition(t);
}

void screen::slider2ValueChanged(phi::eventArgs e)
{
    
    auto pointLight = phi::scenesManager::get()->getScene()->getPointLight(0);
    glm::vec3 pos = pointLight->getPosition();
    auto value =_slider2->getValue();
    glm::vec3 t = glm::vec3(pos.x, value, pos.z);
    pointLight->setPosition(t);

    
    sphere->setPosition(t);
}

void screen::slider3ValueChanged(phi::eventArgs e)
{
    //_sceneRenderer->setSSAOIntensity(_slider3->getValue());
}

void screen::slider4ValueChanged(phi::eventArgs e)
{
    //_sceneRenderer->setSSAOSampleRadius(_slider4->getValue());
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

void screen::selectedSceneObjectChanged(phi::sceneObjectEventArgs e)
{
    if (e.sender->getSelected())
        tc->attachTo(e.sender);
    else
        tc->attachTo(nullptr);
    if (e.sender->getSelected())
        rc->attachTo(e.sender);
    else
        rc->attachTo(nullptr);
}