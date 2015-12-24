#include <phi/loader/importer.h>
#include <phi/demo/screen.h>
#include <phi\demo\screen.h>
#include <phi\diagnostics\diagnostics.h>
#include <phi\diagnostics\stopwatch.h>
#include <phi\core\globals.h>
#include <phi\rendering\model.h>

#include <phi\rendering\shaderManager.h>
#include <GLM\gtc\constants.hpp>

screen::screen() : form()
{
    _temp = 0.0f;
    _isMouseDown = false;
    _rotating = false;
    _shadowMap = false;
    _translationSpeed = 1.0f;
    _rotationSpeed = 0.01f;
    _sceneRenderer = nullptr;
    _library = new library("resources");
}

screen::~screen()
{
}

void screen::initScene()
{
    _library->init();

    _scene = new phi::scene(new phi::camera(0.1f, 1000.0f, getSize(), glm::half_pi<float>()));
    auto camera = _scene->getCamera();

    camera->setLocalPosition(phi::vec3(0.0f, 3.0f, 5.0f));
    camera->setTarget(phi::vec3(0.0f, 0.0f, 0.0f));
    camera->update();

    auto info = phi::shaderManagerInfo();
    info.path = _resourcesPath;
    phi::shaderManager::get()->init(info);

    _sceneRenderer = new phi::sceneRenderer();
    _sceneRenderer->init();

    auto objectRes = _library->getObjectsRepository()->getAllResources()[2];
    _scene->add(objectRes->getObject());
}

void screen::onInitialize()
{
    setTitle("Teste");
    centerScreen();
    initScene();
}

float t = 0.0f;

void screen::update()
{
    t += 0.001f;
    _scene->getCamera()->setLocalPosition(phi::vec3(glm::cos(t), 1.0f, glm::sin(t)) * 1.0f);
    _scene->getCamera()->update();
    _scene->update();
}

void screen::render()
{
    _sceneRenderer->render(_scene);
}

void screen::onResize(SDL_Event e)
{
    auto sz = phi::sizef((float)e.window.data1, (float)e.window.data2);
    setSize(sz);
    _scene->getCamera()->setResolution(sz);
}

void screen::onClosing()
{
    //TODO: MessageBox asking if the user really wants to close the window
    //TODO: Check if we really need the above TODO
}