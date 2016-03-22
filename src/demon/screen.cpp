#include <precompiled.h>
#include "screen.h"

#include <apps\undoCommand.h>
#include <apps\redoCommand.h>

#include <diagnostics\diagnostics.h>
#include <diagnostics\stopwatch.h>

#include <loader\importer.h>
#include <rendering\model.h>
#include <apps\application.h>

#include <ui\floatAnimator.h>

using namespace phi;

namespace demon
{
    screen::screen(string name, uint width, uint height) :
        window(name, width, height)
    {
    }

    screen::~screen()
    {
    }

    void screen::onInit()
    {
        initGL();
        initLibrary();
        initScene();
        initInput();

        //for (float i = 0.0f; i < 1.0f; i += 0.025f)
        //{
        //    int a = (int)(easingFunctions::easeOutBack(i, 1.70158f) * 100.0f);
        //    phi::debug(std::string(a, '-') + "[" + std::to_string(a) + "]");
        //}

        //1.70158f
    }

    void screen::initGL()
    {
        auto initState = gl::state();
        initState.clearColor = vec4(1.0f);
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
        auto cameraPos = vec3(0.0f, 0.0f, 10.0f);
        cameraTransform->setLocalPosition(cameraPos);
        cameraTransform->setDirection(-cameraPos);

        auto floor = _library->getObjectsRepository()->getAllResources()[24]->getObject();
        auto clonedFloor = floor->clone();
        _scene->add(clonedFloor);

        auto obj = _library->getObjectsRepository()->getAllResources()[2]->getObject();
        for (size_t i = 0; i < 1; i++)
        {
            auto cloned = obj->clone();
            cloned->getTransform().setLocalPosition(vec3(i + (0.1f*i), 0.0, 0.0));
            _scene->add(cloned);
        }
    }

    void screen::initInput()
    {
        _commandsManager = new phi::commandsManager();
        _commandsManager->addShortcut(phi::shortcut({ PHIK_CTRL, PHIK_z }, [&]() -> phi::command* { return new phi::undoCommand(_commandsManager); }));
        _commandsManager->addShortcut(phi::shortcut({ PHIK_CTRL, PHIK_y }, [&]() -> phi::command* { return new phi::redoCommand(_commandsManager); }));

        _defaultController = new defaultCameraController(_scene);
    }

    void screen::onUpdate()
    {
        phi::floatAnimator::update();
        _scene->update();
        _defaultController->update();
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
        delete _commandsManager;
        delete _gl;
        delete _library;
        delete _scene;

        //TODO: MessageBox asking if the user really wants to close the window
        //TODO: Check if we really need the above TODO
    }
}