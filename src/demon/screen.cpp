#include <precompiled.h>
#include "screen.h"
#include <diagnostics\diagnostics.h>
#include <diagnostics\stopwatch.h>

#include <loader\importer.h>
#include <rendering\model.h>
#include <apps\application.h>

using namespace phi;

namespace demon
{
    screen::screen(string name, uint width, uint height) :
        window(name, width, height)
    {
    }

    screen::~screen()
    {
        delete _commandsManager;
        delete _inputManager;
        delete _gl;
        delete _library;
        delete _scene;
    }

    void screen::onInit()
    {
        initGL();
        initLibrary();
        initScene();
        initInput();
    }

    void screen::initGL()
    {
        auto initState = gl::state();
        initState.clearColor = vec4(0.0f);
        initState.frontFace = gl::frontFace::ccw;
        initState.culling = true;
        initState.cullFace = gl::cullFace::back;
        initState.depthMask = true;
        initState.depthTest = true;
        initState.useBindlessTextures = false;
        initState.useSparseTextures = false;

        auto info = gl::glInfo();
        info.state = initState;
        info.shadersPath = application::resourcesPath + "/shaders";
        _gl = new gl(info);
    }

    void screen::initLibrary()
    {
        _library = new library(_gl, application::libraryPath);
        _library->init();
    }

    void screen::initScene()
    {
        _scene = new scene(_gl, _width, _height);
        auto camera = _scene->camera;

        auto cameraTransform = camera->getTransform();
        auto cameraPos = vec3(0.0f, 0.0f, 2.0f);
        cameraTransform->setLocalPosition(cameraPos);
        cameraTransform->setDirection(-cameraPos);

        auto floor = _library->getObjectsRepository()->getAllResources()[24]->getObject();
        auto clonedFloor = floor->clone();
        _scene->add(clonedFloor);

        auto obj = _library->getObjectsRepository()->getAllResources()[2]->getObject();
        for (size_t i = 0; i < 10; i++)
        {
            auto cloned = obj->clone();
            cloned->getTransform().setLocalPosition(vec3(i + (0.1f*i), 0.0, 0.0));
            _scene->add(cloned);
        }
    }

    void screen::initInput()
    {
        _commandsManager = new commandsManager();
        _inputManager = new inputManager(_commandsManager);
        _defaultController = new defaultCameraController(_scene);
        _inputManager->setCurrentCameraController(_defaultController);
    }

    void screen::onUpdate()
    {
        //_inputManager->update();
        _scene->update();
    }

    void screen::onRender()
    {
        _scene->render();
    }

    void screen::onTick()
    {
        debug("fps:" + std::to_string(application::framesPerSecond));
    }

    void screen::onClosing()
    {
        debug("closing.");
        //TODO: MessageBox asking if the user really wants to close the window
        //TODO: Check if we really need the above TODO
    }
}