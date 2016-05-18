#include <precompiled.h>
#include "screen.h"

#include <apps\undoCommand.h>
#include <apps\redoCommand.h>

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
    }

    void screen::initGL()
    {
        application::logInfo("Initializing OpenGl");

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
        info.shadersPath = path::combine(application::resourcesPath, "shaders");
        _gl = new gl(info);

        importer::defaultAlbedoTexture = _gl->defaultAlbedoTexture;
        importer::defaultEmissiveTexture = _gl->defaultEmissiveTexture;
        importer::defaultNormalTexture = _gl->defaultNormalTexture;
        importer::defaultSpecularTexture = _gl->defaultSpecularTexture;
        importer::defaultMaterial = _gl->defaultMaterial;

        application::logInfo("Vendor: " + _gl->getVendor() + ".");
        application::logInfo("Renderer: " + _gl->getRenderer() + ".");
        application::logInfo("Version: " + _gl->getVersion() + ".");

        application::logInfo("Extensions:");
        for (auto extensionStatus : _gl->extensions)
        {
            auto status = extensionStatus.second ? " [Ok]" : " [Not Ok]";
            application::logInfo(extensionStatus.first + status);
        }
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

        //auto obj = _library->getObjectsRepository()->getAllResources()[2]->getObject();
        //for (size_t i = 0; i < 1; ++i)
        //{
        //    auto cloned = obj->clone();
        //    cloned->getTransform()->setLocalPosition(vec3(i + (0.1f*i), 0.0, 0.0));
        //    _scene->add(cloned);
        //}

        auto cube = _library->getObjectsRepository()->getAllResources()[7]->getObject()->clone();
        cube->getTransform()->setLocalPosition(vec3(0.0f, 1.0f, 0.0f));
        _scene->add(cube);

        cube = _library->getObjectsRepository()->getAllResources()[7]->getObject()->clone();
        cube->getTransform()->setLocalPosition(vec3(2.9f, 0.9f , 0.0f));
        //cube->getTransform()->setLocalSize(vec3(2.0f));
        _scene->add(cube);

        cube = _library->getObjectsRepository()->getAllResources()[7]->getObject()->clone();
        cube->getTransform()->setLocalPosition(vec3(4.5f, 0.0f, 0.0f));
        cube->getTransform()->setLocalSize(vec3(2.0f));
        _scene->add(cube);

        cube = _library->getObjectsRepository()->getAllResources()[7]->getObject()->clone();
        cube->getTransform()->setLocalPosition(vec3(1.5f, 5.0f, 0.0f));
        cube->getTransform()->roll(PI_OVER_4);
        cube->getTransform()->pitch(PI_OVER_4);
        cube->getTransform()->setLocalSize(vec3(2.0f));
        _scene->add(cube);

        cube = _library->getObjectsRepository()->getAllResources()[7]->getObject()->clone();
        cube->getTransform()->setLocalPosition(vec3(4.5f, 1.0f, 0.0f));
        cube->getTransform()->roll(PI_OVER_4);
        //cube->getTransform()->setLocalSize(vec3(2.0f));
        _scene->add(cube);

        auto floor = _library->getObjectsRepository()->getAllResources()[24]->getObject()->clone();
        floor->getTransform()->yaw(PI_OVER_4);
        _scene->add(floor);

        floor = _library->getObjectsRepository()->getAllResources()[24]->getObject()->clone();
        floor->getTransform()->setLocalPosition(vec3(0.0f, 2.5f, -2.5f));
        floor->getTransform()->pitch(PI_OVER_2);
        _scene->add(floor);
    }

    void screen::initInput()
    {
        _commandsManager = new phi::commandsManager();
        _commandsManager->addShortcut(phi::shortcut({ PHIK_CTRL, PHIK_z }, [&] { return new phi::undoCommand(_commandsManager); }));
        _commandsManager->addShortcut(phi::shortcut({ PHIK_CTRL, PHIK_y }, [&] { return new phi::redoCommand(_commandsManager); }));

        _defaultController = new defaultCameraController(_scene);
    }

    void screen::onUpdate()
    {
        phi::floatAnimator::update();
        _defaultController->update();
        _scene->update();
    }

    void screen::onRender()
    {
        _scene->render();
    }

    void screen::onTick()
    {
        //debug("fps:" + std::to_string(application::framesPerSecond));
#if _DEBUG
        _gl->shadersManager->reloadAllShaders();
#endif
    }

    void screen::onClosing()
    {
        debug("closing.");
        safeDelete(_commandsManager);
        safeDelete(_defaultController);
        safeDelete(_gl);
        safeDelete(_library);
        safeDelete(_scene);

        //TODO: MessageBox asking if the user really wants to close the window
        //TODO: Check if we really need the above TODO
    }
}