﻿#include <phi\demo\screen.h>

#include <phi\diagnostics\diagnostics.h>
#include <phi\diagnostics\stopwatch.h>

#include <phi\core\globals.h>

#include <phi\loader\importer.h>

#include <phi\rendering\model.h>
#include <phi\rendering\shaderManager.h>

#include <phi\scenes\scenesManager.h>

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
    _commandsManager = new commandsManager();
    _inputManager = new inputManager(_commandsManager);
}

screen::~screen()
{
}

void screen::initScene()
{
    _library = new library(_resourcesPath);
    _library->init();

    _scene = new phi::scene(new phi::camera(0.1f, 1000.0f, _size, glm::half_pi<float>()));
    auto camera = _scene->getCamera();

    camera->setLocalPosition(phi::vec3(0.0f, 0.5f, 5.0f));
    camera->setTarget(phi::vec3(0.0f, 0.5f, 0.0f));
    camera->update();

    auto direction = camera->getDirection();
    auto right = camera->getRight();
    auto up = camera->getUp();

    auto info = phi::shaderManagerInfo();
    info.path = _resourcesPath;
    phi::shaderManager::get()->init(info);

    _sceneRenderer = new phi::sceneRenderer(_size);
    _sceneRenderer->init();

    auto obj = _library->getObjectsRepository()->getAllResources()[4]->getObject();
    _scene->add(obj);
}

void screen::onInitialize()
{
    setTitle("Φ");
    centerScreen();

    initScenesManager();
    initScene();

    auto camera = _scene->getCamera();
    _defaultController = new defaultCameraController(camera);
    _inputManager->setCurrentCameraController(_defaultController);
}

void screen::initScenesManager()
{
    phi::scenesManagerInfo info;
    info.applicationPath = getApplicationPath();
    info.resourcesPath = _resourcesPath;
    info.size = getSize();

    phi::scenesManager::get()->init(info);
}

void screen::update()
{
    _scene->getCamera()->update();
    _scene->update();
}

void screen::render()
{
    auto sec = phi::stopwatch::measure([&]
    {
        _sceneRenderer->render(_scene);
    });
    //std::cout << std::to_string(sec * 1000.0f) << std::endl;
}

void screen::onResize(SDL_Event e)
{
    auto sz = phi::sizef((float)e.window.data1, (float)e.window.data2);
    setSize(sz);
    _scene->getCamera()->setResolution(sz);
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
    if(_inputManager->onKeyDown(e))
        return;

    if(e->key == PHIK_ESCAPE)
        close();
}

void screen::onKeyUp(phi::keyboardEventArgs* e)
{
    _inputManager->onKeyUp(e);
}

void screen::onClosing()
{
    //TODO: MessageBox asking if the user really wants to close the window
    //TODO: Check if we really need the above TODO
}