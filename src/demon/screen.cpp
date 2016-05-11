#include <precompiled.h>
#include "screen.h"

#include <diagnostics\stopwatch.h>

#include <loader\importer.h>

#include <core\model.h>

#include <animation\floatAnimator.h>

#include <apps\application.h>
#include <apps\undoCommand.h>
#include <apps\redoCommand.h>

#include <ui\controlRenderer.h>
#include <ui\textRenderer.h>

#include "deleteObjectCommand.h"
#include "addObjectCommand.h"

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
		initAssimp();
        initUserLibrary();
        initScene(); 
        initUi();
        initInput();
    }

    void screen::initGL()
    {
        application::logInfo("Initializing OpenGl");

        auto initState = gl::state();
        initState.clearColor = vec4(1.0f);
        initState.frontFace = gl::frontFace::ccw;
        initState.culling = true;
        initState.cullFace = gl::cullFace::back;
        initState.depthMask = true;
        initState.depthTest = true;
        initState.useBindlessTextures = false;
        initState.useSparseTextures = false;
        initState.swapInterval = 1;

        auto info = gl::glInfo();
        info.state = initState;
        info.shadersPath = application::resourcesPath + "/shaders";
        info.fontsPath = application::resourcesPath + "/fonts";
        _gl = new gl(info);

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

	void screen::initAssimp()
	{
		//auto fileName = path::combine(application::path, "objs\\suzanne.obj");
		//importer::importModel(fileName);
	}

    void screen::initUserLibrary()
    {
		_userLibrary = new library(application::libraryPath);
		_userLibrary->init();
    }

    void screen::initScene()
    {
        _scene = new scene(_gl, static_cast<float>(_width), static_cast<float>(_height));
        auto camera = _scene->getCamera();

        auto cameraTransform = camera->getTransform();
        auto cameraPos = vec3(-3.0f, 0.0f, 10.0f);
        cameraTransform->setLocalPosition(cameraPos);
        cameraTransform->setDirection(-cameraPos);

        auto floor = _userLibrary->getObjectsRepository()->getAllResources()[2]->getObject();
        auto clonedFloor = floor->clone();
        _scene->add(clonedFloor);

        auto obj = _userLibrary->getObjectsRepository()->getAllResources()[0]->getObject();

        auto obj1 = obj->clone();
        obj1->getTransform()->setLocalPosition(vec3(0.f, .5f, 0.f));
        _scene->add(obj1);

		/*auto fileName = path::combine(application::path, "objs\\cubes.dae");
		auto r = importer::importModel(fileName);
		auto o = r->getObject();
		_scene->add(o);*/
    }

    void screen::initUi()
    {
		camera* uiCamera = new camera("uiCamera", static_cast<float>(_width), static_cast<float>(_height), 0.1f, 10000.0f, PI_OVER_4);

        _ui = new ui(uiCamera, _scene->getRenderer(), _gl, static_cast<float>(_width), static_cast<float>(_height));

        auto font = _gl->fontsManager->load("Roboto-Thin.ttf", 14);

        auto label0 = _ui->newLabel(L"nanddiiiiiiiinho", vec3(-100.0f, 0.0f, 0.0f));
        auto controlRenderer = label0->getComponent<phi::controlRenderer>();
        controlRenderer->setColor(color::fromHSL(0.0f, 0.8333f, 0.6667f));

        auto textRenderer = label0->getComponent<phi::textRenderer>();
        textRenderer->setFont(font);
        textRenderer->setColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

        _ui->add(label0);
    }

    void screen::initInput()
    {
        _defaultController = new defaultCameraController(_scene);

        _commandsManager = new commandsManager();
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_z }, [&]() { return new undoCommand(_commandsManager); }));
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_y }, [&]() { return new redoCommand(_commandsManager); }));
        
        _commandsManager->addShortcut(shortcut({ PHIK_DELETE }, [&]() 
        {
            return new deleteObjectCommand(_scene, _defaultController->getSelectionMouseController()); 
        }));
    }

    void screen::onUpdate()
    {
        phi::floatAnimator::update();
        _defaultController->update();
        _scene->update();
        _ui->update();
    }

    void screen::onRender()
    {
        _scene->render();
        _ui->render();
    }

    void screen::onTick()
    {
        //debug("fps: " + std::to_string(application::framesPerSecond));
#if _DEBUG
        _gl->shadersManager->reloadAllShaders();
#endif
    }

    void screen::onClosing()
    {
        safeDelete(_commandsManager);
        safeDelete(_defaultController);
        safeDelete(_gl);
        safeDelete(_userLibrary);
        safeDelete(_scene);
        safeDelete(_ui);
    }
}