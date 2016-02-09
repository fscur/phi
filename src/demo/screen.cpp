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

    camera->setLocalPosition(phi::vec3(7.0f, 3.0f, 5.0f));
    camera->setTarget(phi::vec3(0.0f, 0.0f, 0.0f));
    camera->update();

    auto info = phi::shaderManagerInfo();
    info.path = _resourcesPath;
    phi::shaderManager::get()->init(info);

    auto obj = _library->getObjectsRepository()->getAllResources()[2]->getObject();
    for (size_t i = 0; i < 10; i++)
    {
        auto cloned = obj->clone();
        cloned->setLocalPosition(phi::vec3(i, 0.0, 0.0));
        cloned->update();
        _scene->add(cloned);
    }
}

void screen::onInitialize()
{
    setTitle("phi");
    centerScreen();

    initScenesManager();
    initScene();

    auto camera = _scene->getCamera();
    _defaultController = new defaultCameraController(camera);
    _inputManager->setCurrentCameraController(_defaultController);

    _renderer = new phi::renderer();

    auto frameUniformBlock = phi::frameUniformBlock();
    frameUniformBlock.projectionMatrix = camera->getProjectionMatrix();
    frameUniformBlock.viewMatrix = camera->getViewMatrix();

    auto renderInfo = phi::renderInfo();
    renderInfo.materials = _library->getMaterialsRepository()->getAllObjects();
    renderInfo.renderList = _scene->getRenderList();
    renderInfo.frameUniformBlock = frameUniformBlock;

    _renderer->init(renderInfo);
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
    _inputManager->update();

    auto camera = _scene->getCamera();
    camera->update();

    _scene->update();

    auto frameUniformBlock = phi::frameUniformBlock();
    frameUniformBlock.projectionMatrix = camera->getProjectionMatrix();
    frameUniformBlock.viewMatrix = camera->getViewMatrix();
    _renderer->updateFrameUniformsBuffer(frameUniformBlock);
}

void screen::render()
{
    _renderer->render();
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