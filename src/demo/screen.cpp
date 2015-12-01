#include <phi/rendering/renderingSystem.h>
#include <phi/rendering/renderingSystem.h>
#include <phi/rendering/renderingCommunicationBuffer.h>

#include <phi/scenes/box.h>
#include <phi/scenes/plane.h>
#include <phi/scenes/sphere.h>
#include <phi/scenes/cone.h>
#include <phi/scenes/fsSceneRenderer.h>
#include <phi/scenes/dsSceneRenderer.h>

#include <phi/ui/colorAnimator.h>
#include <phi/ui/uiRepository.h>
#include <phi/ui/uiSystem.h>

#include <phi/demo/screen.h>
#include <phi/demo/translateObjectCommand.h>
#include <phi/demo/rotateObjectCommand.h>
#include <phi/demo/setSceneObjectVisibilityCommand.h>
#include <phi/demo/multiCommand.h>
#include <phi/demo/selectObjectCommand.h>
#include <phi/demo/setMaterialCommand.h>
#include <phi/demo/zoomToFitCommand.h>
#include <phi/demo/addSceneObjectCommand.h>
#include <phi/demo/SEImporter.h>

#include <bullet/btBulletDynamicsCommon.h>

float a;
float angle;
phi::translationControl* tc;
phi::rotationControl* rc;
phi::button3D* fb;
float t = 0.0f;
phi::sceneObject* sphere;
phi::carouselList* _materialsCarousel;
phi::sceneObject* casket0;
phi::sceneObject* cube;
bool oi = false;

screen::screen() : form()
{
    _temp = 0.0f;
    _isMouseDown = false;
    _rotating = false;
    _shadowMap = false;
    _translationSpeed = 1.0f;
    _rotationSpeed = 0.01f;
    _sceneRenderer = nullptr;
    _commandsManager = new commandsManager();
    _inputManager = new inputManager(_commandsManager);
}

screen::~screen()
{
    phi::scenesManager::get()->release();
}

void screen::initScenesManager()
{
    phi::scenesManagerInfo info;
    info.applicationPath = getApplicationPath();
    info.resourcesPath = _resourcesPath;
    info.size = getSize();

    phi::scenesManager::get()->init(info);
}

void screen::initScene()
{
    phi::scene* s = new phi::scene();
    s->setSize(getSize());
    s->getActiveCamera()->setLocalPosition(glm::vec3(0.0f, 0.5f, 5.0f));
    s->getActiveCamera()->setTarget(glm::vec3(0.0f, 0.5f, 0.0f));
    s->setAmbientLightColor(phi::color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));
    s->getSelectedSceneObjectChanged()->bind<screen, &screen::selectedSceneObjectChanged>(this);
    s->getStaticObjectsChanged()->bind<screen, &screen::staticObjectsChanged>(this);

    ///* decoration */

    phi::sceneObject* flower = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Decorations\\flower.model"));
    flower->setLocalPosition(glm::vec3(2.0f, 0.8f, 0.0f));
    s->add(flower);

    phi::sceneObject* portrait = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Decorations\\portrait.model"));
    portrait->setSize(phi::size<float>(2.0f, 2.0f, 1.0f));
    portrait->setLocalPosition(glm::vec3(-4.98f, 2.0f, 0.0f));
    portrait->yaw(phi::PI_OVER_2);
    s->add(portrait);

    phi::sceneObject* floor_lamp = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Lamps\\floor_lamp.model"));
    floor_lamp->setLocalPosition(glm::vec3(-4.7f, 0.0f, -1.7f));
    s->add(floor_lamp);

    phi::pointLight* pointLight_lamp = new phi::pointLight(glm::vec3(-4.7f, 1.0f, -1.7f), phi::color::fromRGBA(1.0f, 0.9f, 0.7f, 1.0f), 0.8f, 10.0f);
    //s->add(pointLight_lamp);

    phi::sceneObject* telephone = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Decorations\\telephone.model"));
    telephone->setLocalPosition(glm::vec3(4.7f, 0.76f, 0.5f));
    telephone->yaw(-phi::PI_OVER_2);
    s->add(telephone);

    phi::sceneObject* pair_vase = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Decorations\\pair_vase.model"));
    pair_vase->setLocalPosition(glm::vec3(4.7f, 0.76f, -0.05f));
    s->add(pair_vase);

    phi::sceneObject* flower_vase = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Decorations\\flower_vase.model"));
    flower_vase->setLocalPosition(glm::vec3(4.7f, 0.76f, -0.20f));
    flower_vase->yaw(-1.5);
    s->add(flower_vase);

    phi::sceneObject* rug = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Decorations\\persian_rug.model"));
    rug->setLocalPosition(glm::vec3(-3.0f, 0.0f, -0.5f));
    s->add(rug);

    phi::sceneObject* rug1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Decorations\\persian_rug_1.model"));
    rug1->setSize(phi::size<float>(1.25f, 1.0f, 1.25f));
    rug1->setLocalPosition(glm::vec3(2.0f, 0.0f, 0.0f));
    s->add(rug1);

    casket0 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Decorations\\casket.model"));
    casket0->setLocalPosition(glm::vec3(-1.0f, 0.0f, 1.5f));
    casket0->yaw(phi::PI + phi::PI_OVER_2);
    s->add(casket0);

    ///* living room */

    phi::sceneObject* sofa = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Chairs\\sofa.model"));
    sofa->setLocalPosition(glm::vec3(-3.1f, 0.0f, 1.4f));
    sofa->yaw(phi::PI);
    s->add(sofa);

    phi::sceneObject* chair0 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Chairs\\brown_chair.model"));
    chair0->setLocalPosition(glm::vec3(-3.8f, 0.0f, -1.0f));
    chair0->yaw(-phi::PI_OVER_2 + 0.3f);
    s->add(chair0);

    phi::sceneObject* chair1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Chairs\\black_chair.model"));
    chair1->setLocalPosition(glm::vec3(-2.3f, 0.0f, -1.0f));
    chair1->yaw(-phi::PI_OVER_2 - 0.3f);
    s->add(chair1);


    /* dining */

    phi::sceneObject* cabinet = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Cabinets\\cabinet.model"));
    cabinet->setLocalPosition(glm::vec3(4.7f, 0.0f, 0.0f));
    cabinet->yaw(phi::PI);
    s->add(cabinet);

    phi::sceneObject* table = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Tables\\table2.model"));
    table->setLocalPosition(glm::vec3(2.0f, 0.0f, 0.0f));
    s->add(table);

    phi::sceneObject* table_chair0 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Chairs\\table_chair.model"));
    table_chair0->setLocalPosition(glm::vec3(1.5f, 0.0f, -0.5f));
    s->add(table_chair0);

    phi::sceneObject* table_chair1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Chairs\\table_chair.model"));
    table_chair1->setLocalPosition(glm::vec3(2.5f, 0.0f, -0.5f));
    table_chair1->yaw(-0.2f);
    s->add(table_chair1);

    phi::sceneObject* table_chair2 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Chairs\\table_chair.model"));
    table_chair2->setLocalPosition(glm::vec3(1.5f, 0.0f, 0.5f));
    table_chair2->yaw(phi::PI - 0.3f);
    s->add(table_chair2);

    phi::sceneObject* table_chair3 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Chairs\\table_chair.model"));
    table_chair3->setLocalPosition(glm::vec3(2.5f, 0.0f, 0.5f));
    table_chair3->yaw(phi::PI + 0.4f);
    s->add(table_chair3);

    phi::sceneObject* table_chair4 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Chairs\\table_chair.model"));
    table_chair4->setLocalPosition(glm::vec3(3.0f, 0.0f, 0.1f));
    table_chair4->yaw(phi::PI_OVER_2 + phi::PI + 0.1f);
    s->add(table_chair4);

    phi::sceneObject* table_chair5 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Chairs\\table_chair.model"));
    table_chair5->setLocalPosition(glm::vec3(0.8f, 0.0f, 0.0f));
    table_chair5->yaw(phi::PI_OVER_2 - 0.1f);
    s->add(table_chair5);



    ///* slider light */
    //
    //phi::pointLight* pointLight = new phi::pointLight(glm::vec3(-8.0f, 2.0f, 0.0f), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 1.4f, 5.0f);
    //s->add(pointLight);

    ///* lights */

    phi::sceneObject* spotLamp0 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Lamps\\spot_lamp.model"));
    spotLamp0->setLocalPosition(glm::vec3(-4.7f, 3.5f, -0.75f));
    spotLamp0->yaw(-phi::PI_OVER_2 + 0.3f);
    s->add(spotLamp0);

    phi::spotLight* spotLight0 = new phi::spotLight(glm::vec3(0.0f, 0.0f, 0.0f), phi::color::fromRGBA(1.0f, 0.7f, 0.5f, 1.0f), 2.5f, 4.0f, glm::vec3(-0.25, -0.8, 0.8), 0.7f);
    spotLamp0->addChild(spotLight0);
    s->add(spotLight0);


    phi::sceneObject* spotLamp1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Lamps\\spot_lamp.model"));
    spotLamp1->setLocalPosition(glm::vec3(-4.7f, 3.5f, 0.75f));
    spotLamp1->yaw(-phi::PI_OVER_2 - 0.3f);
    s->add(spotLamp1);

    phi::spotLight* spotLight1 = new phi::spotLight(glm::vec3(0.0f, 0.0f, 0.0f), phi::color::fromRGBA(1.0f, 0.7f, 0.5f, 1.0f), 2.5f, 4.0f, glm::vec3(-0.25, -0.8, 0.8), 0.7f);
    spotLamp1->addChild(spotLight1);
    s->add(spotLight1);

    phi::sceneObject* lamp1 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Lamps\\hanging_lamp.model"));
    lamp1->setLocalPosition(glm::vec3(2.0f, 3.5f, 0.0f));
    s->add(lamp1);

    phi::spotLight* spotLight2 = new phi::spotLight(glm::vec3(0.0f, -1.6f, 0.0f), phi::color::fromRGBA(1.0f, 0.7f, 0.5f, 1.0f), 2.5f, 5.0f, glm::vec3(0.0f, -1.0f, 0.0f), 0.3f);
    s->add(spotLight2);
    lamp1->addChild(spotLight2);

    //phi::pointLight* pointLight1 = new phi::pointLight(glm::vec3(2.0, 1.9, 0.0), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.8f, 5.0f);
    //s->add(pointLight1);

    phi::sceneObject* lamp2 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Lamps\\ceiling_lamp.model"));
    lamp2->setLocalPosition(glm::vec3(-2.5f, 3.5f, 0.0f));
    s->add(lamp2);

    //phi::pointLight* pointLight2 = new phi::pointLight(glm::vec3(0.0, -1.1, 0.0), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.5f, 10.0f);
    //s->add(pointLight2);
    //lamp2->addChild(pointLight2);

    phi::spotLight* spotLight3 = new phi::spotLight(glm::vec3(0.0f, -1.1f, 0.0f), phi::color::fromRGBA(1.0f, 0.7f, 0.5f, 1.0f), 1.5f, 5.0f, glm::vec3(0.0f, -1.0f, 0.0f), 0.1f);
    s->add(spotLight3);
    lamp2->addChild(spotLight3);

    phi::spotLight* spotLight4 = new phi::spotLight(glm::vec3(0.0f, -1.1f, 0.0f), phi::color::fromRGBA(1.0f, 0.7f, 0.5f, 1.0f), 0.5f, 3.0f, glm::vec3(0.0f, 1.0f, 0.0f), 0.1f);
    s->add(spotLight4);
    lamp2->addChild(spotLight4);

    ///* room */
    //
    phi::sceneObject* floor = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Structures\\floor.model"));
    floor->setLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    s->add(floor);

    phi::sceneObject* ceiling = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Structures\\ceiling.model"));
    ceiling->setLocalPosition(glm::vec3(0.0f, 3.5f, 0.0f));
    ceiling->setSize(phi::size<float>(1.0f, 2.0f, 1.0f));
    s->add(ceiling);

    phi::sceneObject* left_wall = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Structures\\side_wall.model"));
    left_wall->setLocalPosition(glm::vec3(-5.0f, 0.0f, 0.0f));
    s->add(left_wall);

    phi::sceneObject* right_wall = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Structures\\side_wall.model"));
    right_wall->setLocalPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    right_wall->yaw(phi::PI);
    s->add(right_wall);

    phi::sceneObject* back_wall = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Structures\\back_wall.model"));
    back_wall->setLocalPosition(glm::vec3(0.0f, 0.0f, -2.0f));
    s->add(back_wall);

    phi::sceneObject* front_wall = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Structures\\front_wall.model"));
    front_wall->setLocalPosition(glm::vec3(0.0f, 0.0f, 2.0f));
    //s->add(front_wall);

    /*casket0 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("casket.model"));
    casket0->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    casket0->yaw(phi::PI + phi::PI_OVER_2);
    s->add(casket0);*/

    auto pos = glm::vec3(5.0f, 3.0f, -5.0f);
    sphere = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("Decorations\\sphere.model"));
    sphere->setLocalPosition(pos);
    sphere->setSize(phi::size<float>(0.4f, 0.4f, 0.4f));
    s->add(sphere);

    phi::directionalLight* dirLight0 = new phi::directionalLight(glm::vec3(0.0), phi::color::fromRGBA(1.0f, 0.9f, 0.7f, 1.0f), 2.5f, -pos);
    s->add(dirLight0);
    sphere->addChild(dirLight0);

    /*pos = glm::vec3(5.0, 5.0, 5.0);
    phi::directionalLight* dirLight1 = new phi::directionalLight(pos, phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.4f, -pos);
    s->add(dirLight1);

    pos = glm::vec3(-5.0, 5.0, 5.0);
    phi::directionalLight* dirLight2 = new phi::directionalLight(pos, phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.4f, -pos);
    s->add(dirLight2);

    pos = glm::vec3(-5.0, 5.0, -5.0);
    phi::directionalLight* dirLight3 = new phi::directionalLight(pos, phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 0.4f, -pos);
    s->add(dirLight3);*/

    //auto dir = glm::vec3(1.0, -1.0, 0.0);
    //phi::spotLight* spotLight0 = new phi::spotLight(glm::vec3(-2.0f, 2.0f, 0.0f), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 1.0f, 5.0f, dir, 0.9f);
    //s->add(spotLight0);

    /*phi::pointLight* pointLight0 = new phi::pointLight(glm::vec3(0.0f, 2.0f, 0.0f), phi::color::fromRGBA(1.0, 0.9, 0.7, 1.0), 1.0f, 5.0f);
    s->add(pointLight0);*/

    /*phi::sceneObject* cube0 = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("cube.model"));
    cube0->setPosition(glm::vec3(-2.0f, 0.5f, 0.0f));
    s->add(cube0);

    cube = phi::sceneObject::create(_modelsRepository->getResource<phi::model>("cube.model"));
    cube->setPosition(glm::vec3(0.0f, 0.5f, 0.0f));
    s->add(cube);*/

    phi::scenesManager::get()->addScene(s);
    phi::scenesManager::get()->loadScene(0);
}

void screen::initUI()
{
    phi::uiSystemInfo info = phi::uiSystemInfo();
    info.applicationPath = getApplicationPath();
    info.resourcesPath = _resourcesPath;
    info.size = getSize();
    phi::uiSystem::get()->init(info);

    phi::color labelBackground = phi::color::transparent;
    phi::color labelForeground = phi::color::white;

    phi::button* closeButton = new phi::button(getSize());
    closeButton->setText("X");
    closeButton->setToolTipText("Close");
    closeButton->setBackgroundColor(phi::color::fromRGBA(0.7f, 0.7f, 0.7f, 0.5f));
    closeButton->setForegroundColor(phi::color::white);
    closeButton->setSize(phi::size<GLuint>(30, 30));
    closeButton->setX(getSize().width - 30);
    closeButton->setY(0);
    closeButton->getClick()->bind<screen, &screen::closeButtonClick>(this);
    phi::uiSystem::get()->addControl(closeButton);

    phi::button* buttonA = new phi::button(getSize());
    buttonA->setText("Hide selected");
    buttonA->setToolTipText("Hide selected object");
    buttonA->setBackgroundColor(phi::color::fromRGBA(0.7f, 0.7f, 0.7f, 0.5f));
    buttonA->setForegroundColor(phi::color::white);
    buttonA->setSize(phi::size<GLuint>(200, 30));
    buttonA->setZIndex(20.0f);
    buttonA->setX(10);
    buttonA->setY(10);
    buttonA->getClick()->bind<screen, &screen::hideSelectedButtonClick>(this);

    phi::button* buttonB = new phi::button(getSize());
    buttonB->setText("Show all");
    buttonB->setToolTipText("Show all hidden objects");
    buttonB->setBackgroundColor(phi::color::fromRGBA(0.7f, 0.7f, 0.7f, 0.5f));
    buttonB->setForegroundColor(phi::color::white);
    buttonB->setSize(phi::size<GLuint>(200, 30));
    buttonB->setZIndex(20.0f);
    buttonB->setX(10);
    buttonB->setY(50);
    buttonB->getClick()->bind<screen, &screen::showAllButtonClick>(this);

    phi::button* buttonC = new phi::button(getSize());
    buttonC->setText("Import");
    buttonC->setToolTipText("Import Engineering Files");
    buttonC->setBackgroundColor(phi::color::fromRGBA(0.7f, 0.7f, 0.7f, 0.5f));
    buttonC->setForegroundColor(phi::color::white);
    buttonC->setSize(phi::size<GLuint>(200, 30));
    buttonC->setZIndex(20.0f);
    buttonC->setX(10);
    buttonC->setY(90);
    buttonC->getClick()->bind<screen, &screen::importSEButtonClick>(this);

    _labelFps = new phi::label(getSize());
    _labelFps->setBackgroundColor(phi::color::fromRGBA(0.95f, 0.95f, 0.95f, 0.5f));
    _labelFps->setForegroundColor(phi::color::white);
    _labelFps->setText("No FPS");
    _labelFps->setToolTipText("Current frames per second");
    _labelFps->setSize(phi::size<GLuint>(30, 30));
    _labelFps->setX(getSize().width - 60);
    _labelFps->setY(0);

    //phi::toggleButton* buttonC = new phi::toggleButton(getSize());
    //buttonC->setText("Use esse \"template\" para criar um toggleButton ;)");
    //buttonC->setToolTipText("Hehe :)");
    //buttonC->setBackgroundColor(phi::color::fromRGBA(1.0f, 0.5f, 0.5f, 1.0f));
    //buttonC->setForegroundColor(phi::color::white);
    //buttonC->setCheckedColor(phi::color::fromRGBA(1.0f, 0.2f, 0.2f, 1.0f));
    //buttonC->setSize(phi::size<GLuint>(200, 30));
    //buttonC->setZIndex(20.0f);
    //buttonC->setX(10);
    //buttonC->setY(90);

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

    _materialsCarousel = new phi::carouselList(getSize());
    _materialsCarousel->setBackgroundColor(phi::color::fromRGBA(0.5, 0.5, 0.5, 0.5));
    _materialsCarousel->setSize(phi::size<unsigned int>(getSize().width, 150));
    _materialsCarousel->setX(0);
    _materialsCarousel->setY(getSize().height - 150);
    _materialsCarousel->setZIndex(0);
    _materialsCarousel->setExpanded(false);

    auto materialsDirs = _materialsRepository->getDirectoriesFromPath("");
    auto materialsTabIndex = _materialsCarousel->addTab("Materials", phi::uiRepository::repository->getResource<phi::texture>("materials.png"));
    for (unsigned int i = 0; i < materialsDirs.size(); i++)
    {
        auto tab = new phi::carouselTab(getSize());
        tab->setName(materialsDirs[i]);
        tab->setImage(phi::uiRepository::repository->getResource<phi::texture>("categories/" + materialsDirs[i] + ".png"));
        _materialsCarousel->addSubTab(materialsTabIndex, tab);

        auto materials = _materialsRepository->getResourcesFromPath<phi::material>(materialsDirs[i]);
        for (unsigned int j = 0; j < materials.size(); j++)
        {
            auto material = materials[j];
            phi::carouselItem* carouselItem = new phi::carouselItem(material->getFullName(), getSize());
            carouselItem->setImage(material->getThumbnail());
            carouselItem->setToolTipText(material->getName());
            carouselItem->getClick()->bind<screen, &screen::carouselListItemClick>(this);
            tab->addCarouselItem(carouselItem);
        }
    }

    auto modelsDirs = _modelsRepository->getDirectoriesFromPath("");
    auto modelsTabIndex = _materialsCarousel->addTab("Models", phi::uiRepository::repository->getResource<phi::texture>("models.png"));
    for (unsigned int i = 0; i < modelsDirs.size(); i++)
    {
        auto tab = new phi::carouselTab(getSize());
        tab->setName(modelsDirs[i]);
        _materialsCarousel->addSubTab(modelsTabIndex, tab);

        auto models = _modelsRepository->getResourcesFromPath<phi::model>(modelsDirs[i]);
        for (unsigned int j = 0; j < models.size(); j++)
        {
            auto model = models[j];
            phi::carouselItem* carouselItem = new phi::carouselItem(model->getFullName(), getSize());
            carouselItem->setImage(model->getThumbnail());
            carouselItem->setToolTipText(model->getName());
            carouselItem->setDragData(model->getFullName());
            carouselItem->setDragTexture(model->getThumbnail());
            tab->addCarouselItem(carouselItem);
        }
    }

    auto camera = phi::scenesManager::get()->getScene()->getActiveCamera();

    tc = new phi::translationControl(getSize());
    tc->setCamera(phi::scenesManager::get()->getScene()->getActiveCamera());
    tc->getTranslating()->bind<screen, &screen::translating>(this);
    tc->getTranslationFinished()->bind<screen, &screen::translationFinished>(this);
    phi::uiSystem::get()->addControl(tc);

    rc = new phi::rotationControl(getSize());
    rc->setCamera(phi::scenesManager::get()->getScene()->getActiveCamera());
    rc->getRotating()->bind<screen, &screen::rotating>(this);
    rc->getRotationFinished()->bind<screen, &screen::rotationFinished>(this);
    phi::uiSystem::get()->addControl(rc);

    fb = new phi::button3D(getSize());
    fb->setCamera(phi::scenesManager::get()->getScene()->getActiveCamera());
    phi::uiSystem::get()->addControl(fb);

    phi::uiSystem::get()->addControl(buttonA);
    phi::uiSystem::get()->addControl(buttonB);
    phi::uiSystem::get()->addControl(buttonC);
    //phi::uiSystem::get()->addControl(buttonC);
    //phi::uiSystem::get()->addControl(_labelFps);
    //phi::uiSystem::get()->addControl(_slider1);
    //phi::uiSystem::get()->addControl(_slider2);
    //phi::uiSystem::get()->addControl(_slider3);
    //phi::uiSystem::get()->addControl(_slider4);
    phi::uiSystem::get()->addControl(_materialsCarousel);
    //phi::uiSystem::get()->addControl(textBoxA);

    phi::uiSystem::get()->resize(getSize());

    phi::uiSystem::get()->getControlGotFocus()->bind<screen, &screen::hudControlGotFocus>(this);
    phi::uiSystem::get()->getControlLostFocus()->bind<screen, &screen::hudControlLostFocus>(this);
}

void screen::onInitialize()
{
    setTitle("Teste");
    centerScreen();

    if (_resourcesPath.empty())
        _resourcesLoader.load();
    else
        _resourcesLoader.load(_resourcesPath);

    _modelsRepository = _resourcesLoader.getModels();
    _materialsRepository = _resourcesLoader.getMaterias();
    _texturesRepository = _resourcesLoader.getTextures();
    initScenesManager();
    initScene();
    initUI();
    phi::dragDropController::get()->getDradDropEnded()->bind<screen, &screen::dragDropEnded>(this);
    _fpsController = new fpsCameraController(getSize());
    _defaultController = new defaultCameraController(getSize());
    _inputManager->setCurrentCameraController(_defaultController);
}

void screen::update()
{
    _sceneRenderer = phi::scenesManager::get()->getSceneRenderer();

    a += 0.01f;
    if (a > 2 * phi::PI)
        a -= 2 * phi::PI;

    if (t > 24.0f)
        t -= 24.0f;

    t += 0.01f;

    if (oi)
    {
        auto pos = casket0->getPosition();
        casket0->setLocalPosition(glm::vec3(pos.x + 0.001, pos.y, pos.z));
    }

    phi::scenesManager::get()->update();
    phi::colorAnimator::update();
    phi::uiSystem::get()->update();
    _inputManager->update();
    _commandsManager->update();

    _labelFps->setText(std::to_string(getFps()));

    /*_labelDt->setText("TotalTime/Frame: " + std::to_string(getDt()));
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

    //Draw AABB:
    //auto camera = phi::scenesManager::get()->getScene()->getActiveCamera();
    //auto shader = phi::shaderManager::get()->getShader("UI_MESH");
    //auto objs = *phi::scenesManager::get()->getScene()->getAllObjects();
    //for (auto so : objs)
    //{
    //    auto rd = so->getRigidBody();
    //    auto sh = so->getShape();
    //    btVector3 btMin, btMax;
    //    rd->getAabb(btMin, btMax);
    //    //sh->getAabb(btTransform(), btMin, btMax);
    //    auto min = glm::vec3(btMin.getX(), btMin.getY(), btMin.getZ());
    //    auto max = glm::vec3(btMax.getX(), btMax.getY(), btMax.getZ());
    //    //auto min = _object->getAabb()->getMin();
    //    //auto max = _object->getAabb()->getMax();
    //    auto lbb = glm::vec3(min.x, min.y, min.z);
    //    auto lbf = glm::vec3(min.x, min.y, max.z);
    //    auto ltf = glm::vec3(min.x, max.y, max.z);
    //    auto ltb = glm::vec3(min.x, max.y, min.z);
    //    auto rbb = glm::vec3(max.x, min.y, min.z);
    //    auto rbf = glm::vec3(max.x, min.y, max.z);
    //    auto rtf = glm::vec3(max.x, max.y, max.z);
    //    auto rtb = glm::vec3(max.x, max.y, min.z);

    //    auto mvp = camera->getPerspProjMatrix() * camera->getViewMatrix();
    //    shader->bind();
    //    shader->setUniform("mvp", mvp);
    //    shader->setUniform("color", phi::color::orange);
    //    auto pos = std::vector<glm::vec3>();
    //    pos.push_back(lbb);
    //    pos.push_back(lbf);
    //    pos.push_back(ltf);
    //    pos.push_back(ltb);
    //    pos.push_back(rbb);
    //    pos.push_back(rbf);
    //    pos.push_back(rtf);
    //    pos.push_back(rtb);
    //    auto ind = new std::vector<GLuint>();
    //    ind->push_back(0);
    //    ind->push_back(1);
    //    ind->push_back(1);
    //    ind->push_back(2);
    //    ind->push_back(2);
    //    ind->push_back(3);
    //    ind->push_back(3);
    //    ind->push_back(0);
    //    ind->push_back(4);
    //    ind->push_back(5);
    //    ind->push_back(5);
    //    ind->push_back(6);
    //    ind->push_back(6);
    //    ind->push_back(7);
    //    ind->push_back(7);
    //    ind->push_back(4);
    //    ind->push_back(0);
    //    ind->push_back(4);
    //    ind->push_back(1);
    //    ind->push_back(5);
    //    ind->push_back(2);
    //    ind->push_back(6);
    //    ind->push_back(3);
    //    ind->push_back(7);

    //    auto a = phi::lineMesh::create("oi", pos, ind);
    //    a->bind();
    //    a->render();
    //    a->unbind();

    //    DELETE(a);
    //}
    //shader->unbind();
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
}

void screen::onMouseDown(phi::mouseEventArgs* e)
{
    _inputManager->onMouseDown(e);
}

void screen::onMouseMove(phi::mouseEventArgs* e)
{
    _inputManager->onMouseMove(e);
}

void screen::onMouseUp(phi::mouseEventArgs* e)
{
    _inputManager->onMouseUp(e);
}

void screen::onMouseWheel(phi::mouseEventArgs* e)
{
    _inputManager->onMouseWheel(e);
}

void screen::onKeyDown(phi::keyboardEventArgs* e)
{
    if (_inputManager->onKeyDown(e))
        return;

    if (e->key == PHIK_ESCAPE)
        close();
}

void screen::onKeyUp(phi::keyboardEventArgs* e)
{
    _inputManager->onKeyUp(e);
}

void screen::onEndInput()
{
}

void screen::onClosing()
{
    //TODO: MessageBox asking if the user really wants to close the window
}

void screen::hideSelectedButtonClick(phi::mouseEventArgs* e)
{
    auto cmds = std::vector<command*>();
    cmds.push_back(new selectObjectCommand(nullptr, nullptr));

    auto allObjects = phi::scenesManager::get()->getScene()->getAllObjects();
    for (auto i = 0; i < allObjects->size(); i++)
    {
        auto obj = allObjects->at(i);
        if (obj->getSelected())
            cmds.push_back(new setSceneObjectVisibilityCommand(obj, false));
    }

    auto multiCmd = new multiCommand(cmds);
    _commandsManager->executeCommand(multiCmd);
}

void screen::showAllButtonClick(phi::mouseEventArgs* e)
{
    auto cmds = std::vector<command*>();

    auto allObjects = phi::scenesManager::get()->getScene()->getAllObjects();
    for (auto i = 0; i < allObjects->size(); i++)
    {
        auto obj = allObjects->at(i);
        if (!obj->getActive())
            cmds.push_back(new setSceneObjectVisibilityCommand(obj, true));
    }

    auto multiCmd = new multiCommand(cmds);
    _commandsManager->executeCommand(multiCmd);
}

void screen::importSEButtonClick(phi::mouseEventArgs* e)
{
#ifdef WIN32

    OPENFILENAMEA ofn = { 0 };
    char buffer[300];
    memset(buffer, '\0', 300);
    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = this->getHwnd();
    ofn.lpstrFile = buffer;
    ofn.nMaxFile = 300;
    ofn.Flags = OFN_EXPLORER;
    ofn.lpstrFilter = "Supported Files(*.par, *.psm, *.asm, *.jt)\0 * .par; *.psm; *.asm; *.jt\0Part files (*.par)\0 * .par\0Sheet Metal files (*.psm)\0 * .psm\0Assembly files (*.asm)\0 * .asm\0Jt files (*.jt)\0 * .jt\0";
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = NULL;

    GetOpenFileNameA(&ofn);

    std::cout << buffer << (int)CommDlgExtendedError() << std::endl;

    auto sePartImporter = new SEImporter();
    auto sceneObject = sePartImporter->import(std::string(buffer));

    _commandsManager->executeCommand(new addSceneObjectCommand(sceneObject));

#endif
}

void screen::slider1ValueChanged(phi::eventArgs e)
{
    /*_sceneRenderer->setSSAOBias(_slider1->getValue());
    phi::spotLight* spotLight = phi::scenesManager::get()->getScene()->getSpotLight(0);
    spotLight->setCutoff(_slider1->getValue());*/

    auto pointLight = phi::scenesManager::get()->getScene()->getPointLight(0);
    glm::vec3 pos = pointLight->getPosition();
    auto value = _slider1->getValue();
    glm::vec3 t = glm::vec3(-glm::cos(value) * 8.0f, pos.y, glm::sin(value) * 4.0f);
    pointLight->setLocalPosition(t);


    sphere->setLocalPosition(t);
}

void screen::slider2ValueChanged(phi::eventArgs e)
{

    auto pointLight = phi::scenesManager::get()->getScene()->getPointLight(0);
    glm::vec3 pos = pointLight->getPosition();
    auto value = _slider2->getValue();
    glm::vec3 t = glm::vec3(pos.x, value, pos.z);
    pointLight->setLocalPosition(t);


    sphere->setLocalPosition(t);
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
    //SDL_StartTextInput();
}

void screen::hudControlLostFocus(phi::controlEventArgs e)
{
    //SDL_StopTextInput();
}

void screen::selectedSceneObjectChanged(phi::sceneObjectEventArgs e)
{
    if (e.sender->getSelected())
    {
        tc->attachTo(e.sender);
        rc->attachTo(e.sender);
        //fb->attachTo(e.sender);
    }
    else
    {
        tc->attachTo(nullptr);
        rc->attachTo(nullptr);
        //fb->attachTo(nullptr);
    }

    //if (e.sender->getSelected())
    //{
    //    auto sceneMeshes = e.sender->getSceneMeshes();
    //    auto sceneMeshesCount = sceneMeshes.size();
    //    phi::sceneMesh* uniqueMesh = nullptr;
    //    for (unsigned int i = 0; i < sceneMeshesCount; i++)
    //    {
    //        auto sm = sceneMeshes[i];
    //        if (sm->getIsSelected())
    //        {
    //            if (uniqueMesh)
    //            {
    //                uniqueMesh = nullptr;
    //                break;
    //            }

    //            uniqueMesh = sm;
    //        }
    //    }

    //    if (uniqueMesh != nullptr)
    //    {
    //        auto carouselItems = _materialsCarousel->getItems();
    //        auto carouselItemsCount = carouselItems.size();
    //        for (unsigned int j = 0; j < carouselItemsCount; j++)
    //        {
    //            auto material = uniqueMesh->getMaterial();
    //            auto item = carouselItems[j];
    //            if (item->getName() == material->getFullName())
    //            {
    //                _materialsCarousel->setSelectedItem(item);
    //                break;
    //            }
    //        }
    //    }
    //}
}

void screen::carouselListItemClick(phi::mouseEventArgs* e)
{
    phi::sceneObject* obj = nullptr;
    auto sceneObjects = phi::scenesManager::get()->getScene()->getAllObjects();
    auto sceneObjectsCount = sceneObjects->size();
    for (unsigned int i = 0; i < sceneObjectsCount; i++)
    {
        auto so = (*sceneObjects)[i];
        if (so->getSelected())
        {
            obj = so;
            break;
        }
    }

    if (obj == nullptr)
        return;

    auto cmds = std::vector<command*>();
    auto carouselItem = (phi::carouselItem*)e->sender;
    auto sceneMeshes = obj->getSceneMeshes();
    auto sceneMeshesCount = sceneMeshes.size();
    for (unsigned int i = 0; i < sceneMeshesCount; i++)
    {
        auto sm = sceneMeshes[i];
        if (sm->getIsSelected())
        {
            auto material = _materialsRepository->getResource<phi::material>(carouselItem->getName());
            cmds.push_back(new setMaterialCommand(sm, material));
        }
    }

    auto multiCmd = new multiCommand(cmds);
    _commandsManager->executeCommand(multiCmd);
}

void screen::staticObjectsChanged(phi::eventArgs e)
{
    if (_sceneRenderer)
        _sceneRenderer->redrawStaticShadowMaps();
}

struct contactSensorCallback : public btCollisionWorld::ContactResultCallback
{
    contactSensorCallback(btCollisionObject& tgtBody, screen& context)
        : btCollisionWorld::ContactResultCallback(), body(tgtBody), ctxt(context), collided(false)
    {
    }

    btCollisionObject& body;
    screen& ctxt;
    bool collided;

    virtual bool needsCollision(btBroadphaseProxy* proxy) const
    {
        if (!btCollisionWorld::ContactResultCallback::needsCollision(proxy)) // superclass will check m_collisionFilterGroup and m_collisionFilterMask
            return false;

        return body.checkCollideWithOverride(static_cast<btCollisionObject*>(proxy->m_clientObject)); // if passed filters, may also want to avoid contacts between constraints
    }

    virtual btScalar addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0, int partId0, int index0,
        const btCollisionObjectWrapper* colObj1, int partId1, int index1)
    {
        btVector3 pt; // will be set to point of collision relative to body
        if (colObj0->m_collisionObject == &body)
            pt = cp.m_localPointA;
        else
        {
            assert(colObj1->m_collisionObject == &body && "body does not match either collision object");
            pt = cp.m_localPointB;
        }

        collided = true;
        // do stuff with the collision point
        return 0;
    }
};

void screen::translating(phi::translationEventArgs* e)
{
    // Colis�o usando Bullet:
    auto sceneObj = static_cast<phi::sceneObject*>(e->sender);
    auto scene = phi::scenesManager::get()->getScene();
    scene->getBulletWorld()->updateAabbs();
    sceneObj->getBulletRigidBody()->clearForces();

    contactSensorCallback callback(*sceneObj->getBulletRigidBody(), *this);
    scene->getBulletWorld()->contactTest(sceneObj->getBulletRigidBody(), callback);

    if (callback.collided)
    {
        e->cancel = true;
        //e->valorG�mbisDELETERISSOASSIMQUEPOSSIVEL = callback.mainfoldPoint.getDistance();
        //e->directionDELETERISSOASSIMQUEPOSSIVEL = glm::vec3(callback.mainfoldPoint.m_normalWorldOnB.getX(), callback.mainfoldPoint.m_normalWorldOnB.getY(), callback.mainfoldPoint.m_normalWorldOnB.getZ());
    }

    // Colis�o usando FCL:
    //auto objects = *scene->getAllObjects();
    //auto obj = static_cast<phi::sceneObject*>(e->sender);
    //auto orient = obj->getOrientation();
    //auto pos = e->startPos;
    //auto transf = fcl::Transform3f(fcl::Quaternion3f(orient.w, orient.x, orient.y, orient.z), fcl::Vec3f(pos.x, pos.y, pos.z));
    //auto co = new fcl::CollisionObject(boost::shared_ptr<fcl::CollisionGeometry>(obj->getFclModel()), transf);
    //auto goalTransf = fcl::Transform3f(fcl::Quaternion3f(orient.w, orient.x, orient.y, orient.z), fcl::Vec3f(e->endPos.x, e->endPos.y, e->endPos.z));
    //for each (auto so in objects)
    //{
    //    if (so == e->sender)
    //        continue;

    //    auto orient2 = so->getOrientation();
    //    auto pos2 = so->getPosition();
    //    auto transf2 = fcl::Transform3f(fcl::Quaternion3f(orient2.w, orient2.x, orient2.y, orient2.z), fcl::Vec3f(pos2.x, pos2.y, pos2.z));
    //    auto co2 = new fcl::CollisionObject(boost::shared_ptr<fcl::CollisionGeometry>(so->getFclModel()), transf2);

    //    fcl::ContinuousCollisionRequest request;
    //    request.ccd_solver_type = fcl::CCDC_NAIVE;
    //    request.num_max_iterations = 200;
    //    request.toc_err = 0.00000000001f;
    //    fcl::ContinuousCollisionResult result;
    //    auto a = fcl::continuousCollide(co, goalTransf, co2, transf2, request, result);

    //    if (result.is_collide)
    //    {
    //        auto colPos = glm::vec3(result.contact_tf1.getTranslation()[0], result.contact_tf1.getTranslation()[1], result.contact_tf1.getTranslation()[2]);
    //        auto diff = e->endPos - colPos;
    //        if (colPos == e->startPos)
    //            continue;

    //        e->cancel = true;

    //        LOG(std::to_string(colPos.x) + ";" + std::to_string(colPos.y) + ";" + std::to_string(colPos.z));
    //        e->valorG�mbisDELETERISSOASSIMQUEPOSSIVEL = glm::length(e->endPos - colPos);

    //        return;
    //    }
    //}
}

void screen::translationFinished(phi::translationEventArgs* e)
{
    if (e->startPos == e->endPos)
        return;

    auto cmd = new translateObjectCommand(e->sender, e->startPos, e->endPos);
    _commandsManager->executeCommand(cmd);
}

void screen::rotating(phi::rotationEventArgs* e)
{
    // Colis�o usando Bullet:
    auto sceneObj = static_cast<phi::sceneObject*>(e->sender);
    auto scene = phi::scenesManager::get()->getScene();

    contactSensorCallback callback(*sceneObj->getBulletRigidBody(), *this);
    scene->getBulletWorld()->contactTest(sceneObj->getBulletRigidBody(), callback);
    if (callback.collided)
        e->cancel = true;

    // Colis�o usando FCL:
    //auto objects = *scene->getAllObjects();
    //auto obj = static_cast<phi::sceneObject*>(e->sender);
    //auto orient = e->startOrientation;
    //auto pos = obj->getPosition();
    //auto transf = fcl::Transform3f(fcl::Quaternion3f(orient.w, orient.x, orient.y, orient.z), fcl::Vec3f(pos.x, pos.y, pos.z));
    //auto co = new fcl::CollisionObject(boost::shared_ptr<fcl::CollisionGeometry>(obj->getFclModel()), transf);
    //auto goalTransf = fcl::Transform3f(fcl::Quaternion3f(e->endOrientation.w, e->endOrientation.x, e->endOrientation.y, e->endOrientation.z), fcl::Vec3f(pos.x, pos.y, pos.z));

    //for each (auto so in objects)
    //{
    //    if (so == e->sender)
    //        continue;

    //    auto orient2 = so->getOrientation();
    //    auto pos2 = so->getPosition();
    //    auto transf2 = fcl::Transform3f(fcl::Quaternion3f(orient2.w, orient2.x, orient2.y, orient2.z), fcl::Vec3f(pos2.x, pos2.y, pos2.z));
    //    auto co2 = new fcl::CollisionObject(boost::shared_ptr<fcl::CollisionGeometry>(so->getFclModel()), transf2);

    //    fcl::ContinuousCollisionRequest request;
    //    //request.num_max_iterations = 200;
    //    //request.toc_err = 0.00000000001f;
    //    fcl::ContinuousCollisionResult result;
    //    auto a = fcl::continuousCollide(co, goalTransf, co2, transf2, request, result);

    //    if (result.is_collide)
    //    {
    //        e->cancel = true;
    //        auto fclRot = result.contact_tf1.getRotation();
    //        auto rotMat = glm::mat3(
    //            fclRot.getColumn(0)[0], fclRot.getColumn(0)[1], fclRot.getColumn(0)[2],
    //            fclRot.getColumn(1)[0], fclRot.getColumn(1)[1], fclRot.getColumn(1)[2],
    //            fclRot.getColumn(2)[0], fclRot.getColumn(2)[1], fclRot.getColumn(2)[2]);
    //        e->cancelOrientation = glm::quat(rotMat);
    //        //e->cancelOrientation = glm::quat(result.contact_tf1.getQuatRotation()[0], result.contact_tf1.getQuatRotation()[1], result.contact_tf1.getQuatRotation()[2], result.contact_tf1.getQuatRotation()[3]);
    //        return;
    //    }
    //}
}

void screen::rotationFinished(phi::rotationEventArgs e)
{
    if (e.startOrientation == e.endOrientation)
        return;

    auto cmd = new rotateObjectCommand(e.sender, e.startOrientation, e.endOrientation);
    _commandsManager->executeCommand(cmd);
}

void screen::dragDropEnded(phi::dragDropEventArgs* e)
{
    if (e->handled)
        return;

    auto object = phi::sceneObject::create(_modelsRepository->getResource<phi::model>(e->data));
    auto command = new addSceneObjectCommand(object, glm::vec2(e->x, e->y));
    _commandsManager->executeCommand(command);
}