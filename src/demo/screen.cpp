#include "screen.h"

#include <core\globals.h>
#include <diagnostics\diagnostics.h>
#include <diagnostics\stopwatch.h>

#include <loader\importer.h>
#include <rendering\model.h>

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
    initGL();
    initLibrary();
    initScene();
    initInput();
}

void screen::initGL()
{
    auto initState = phi::gl::state();
    initState.clearColor = phi::vec4(0.0f);
    initState.frontFace = phi::gl::frontFace::ccw;
    initState.culling = true;
    initState.cullFace = phi::gl::cullFace::back;
    initState.depthMask = true;
    initState.depthTest = true;
    initState.useBindlessTextures = false;
    initState.useSparseTextures = false;

    auto info = phi::gl::glInfo();
    info.state = initState;
    info.shadersPath = _resourcesPath + "/shaders";
    _gl = new phi::gl(info);
}

void screen::initLibrary()
{
    _library = new library(_gl, _libraryPath);
    _library->init();
}

void screen::initScene()
{
    _scene = new phi::scene(_gl, _size.w, _size.h);
    auto camera = _scene->camera;

    auto cameraTransform = camera->getTransform();
    auto cameraPos = phi::vec3(0.0f, 0.0f, 2.0f);
    cameraTransform->setLocalPosition(cameraPos);
    cameraTransform->setDirection(-cameraPos);

    auto floor = _library->getObjectsRepository()->getAllResources()[24]->getObject();
    auto clonedFloor = floor->clone();
    _scene->add(clonedFloor);

    auto obj = _library->getObjectsRepository()->getAllResources()[2]->getObject();
    for (size_t i = 0; i < 10; i++)
    {
        auto cloned = obj->clone();
        cloned->getTransform()->setLocalPosition(phi::vec3(i + (0.1f*i), 0.0, 0.0));
        _scene->add(cloned);
    }
}

void screen::initInput()
{
    _defaultController = new defaultCameraController(_scene);
    _inputManager->setCurrentCameraController(_defaultController);
}

void screen::update()
{
    _inputManager->update();
    _scene->update();
}

void screen::render()
{
    _scene->render();
}

void screen::onResize(SDL_Event e)
{
    _scene->camera->setResolution(phi::vec2((float)e.window.data1, (float)e.window.data2));
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

void screen::onClosing()
{
    //TODO: MessageBox asking if the user really wants to close the window
    //TODO: Check if we really need the above TODO
}