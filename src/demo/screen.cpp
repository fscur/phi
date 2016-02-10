#include "screen.h"

#include <diagnostics\diagnostics.h>
#include <diagnostics\stopwatch.h>

#include <core\globals.h>

#include <loader\importer.h>

#include <rendering\model.h>
#include <rendering\shaderManager.h>
#include <rendering\renderingSystem.h>

#include <scenes\scenesManager.h>

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

void screen::onInitialize()
{
    setTitle("phi");
    centerScreen();

    initRenderingSystem();
    initScenesManager();
    initScene();
    initInput();
    initPipeline();
}

void screen::initRenderingSystem()
{
    auto renderingInfo = phi::renderingSystemInfo();
    renderingInfo.applicationPath = getApplicationPath();
    renderingInfo.resourcesPath = _resourcesPath;
    renderingInfo.size = getSize();
    phi::renderingSystem::init(renderingInfo);
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
    _library = new library(_libraryPath);
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

void screen::initInput()
{
    auto camera = _scene->getCamera();
    _defaultController = new defaultCameraController(camera);
    _inputManager->setCurrentCameraController(_defaultController);
}

void screen::initPipeline()
{
    auto camera = _scene->getCamera();
    auto frameUniformBlock = phi::frameUniformBlock();
    frameUniformBlock.p = camera->getProjectionMatrix();
    frameUniformBlock.v = camera->getViewMatrix();
    frameUniformBlock.vp = frameUniformBlock.p * frameUniformBlock.v;

    auto glConfig = phi::gl::config();
    glConfig.clearColor = phi::vec4(1.0f);
    glConfig.frontFace = phi::gl::frontFace::ccw;
    glConfig.culling = true;
    glConfig.cullFace = phi::gl::cullFace::back;
    glConfig.depthMask = true;
    glConfig.depthTest = true;

    auto pipelineInfo = phi::pipelineInfo();
    pipelineInfo.materials = _library->getMaterialsRepository()->getAllObjects();
    pipelineInfo.renderList = _scene->getRenderList();
    pipelineInfo.frameUniformBlock = frameUniformBlock;
    pipelineInfo.config = glConfig;

    phi::renderingSystem::pipeline.init(pipelineInfo);

    _pipeline = &phi::renderingSystem::pipeline;
    _renderer = new phi::renderer();
}

void screen::update()
{
    _inputManager->update();

    auto camera = _scene->getCamera();
    camera->update();

    _scene->update();

    auto frameUniformBlock = phi::frameUniformBlock();
    frameUniformBlock.p = camera->getProjectionMatrix();
    frameUniformBlock.v = camera->getViewMatrix();
    frameUniformBlock.vp = frameUniformBlock.p * frameUniformBlock.v;

    _pipeline->updateFrameUniformBlock(frameUniformBlock);
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