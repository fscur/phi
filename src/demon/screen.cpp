#include <precompiled.h>
#include "screen.h"

#include <apps\undoCommand.h>
#include <apps\redoCommand.h>

#include <diagnostics\stopwatch.h>

#include <loader\importer.h>
#include <rendering\model.h>
#include <apps\application.h>

#include <ui\controlRenderer.h>
#include <ui\textRenderer.h>

#include <animation\floatAnimator.h>

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
        _scene = new scene(_gl, static_cast<float>(_width), static_cast<float>(_height));
        auto camera = _scene->getCamera();

        auto cameraTransform = camera->getTransform();
        auto cameraPos = vec3(-3.0f, 0.0f, 10.0f);
        cameraTransform->setLocalPosition(cameraPos);
        cameraTransform->setDirection(-cameraPos);

        auto floor = _library->getObjectsRepository()->getAllResources()[2]->getObject();
        auto clonedFloor = floor->clone();
        _scene->add(clonedFloor);

        auto obj = _library->getObjectsRepository()->getAllResources()[0]->getObject();
        
        for (auto i = 0; i < 5; ++i)
        {
            auto cloned = obj->clone();
            cloned->getTransform()->setLocalPosition(vec3(i + (0.1f * i), 0.0, 0.0));
            _scene->add(cloned);
        }
    }

    void screen::initUi()
    {
        _ui = new ui(_scene->getRenderer(), _gl, static_cast<float>(_width), static_cast<float>(_height));

        auto font = _gl->fontsManager->load("Roboto-Thin.ttf", 14);

        auto label0 = _ui->newLabel(L"Filipe Scur", vec3(-100.0f, 0.0f, 0.0f));
        auto controlRenderer = label0->getComponent<phi::controlRenderer>();
        controlRenderer->setColor(color::fromRGBA(0.9f, 0.9f, 0.9f, 1.0f));
		controlRenderer->setIsGlassy(true);

        auto textRenderer = label0->getComponent<phi::textRenderer>();
        textRenderer->setFont(font);
        textRenderer->setColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

        _ui->add(label0);

  //      auto label1 = _ui->newLabel(L"JAQUIRANA Ó", vec3(-100.0f, 100.0f, 0.0f));
  //      controlRenderer = label1->getComponent<phi::controlRenderer>();

  //      auto texture = _library->getTexturesRepository()->getAllResources()[0]->getObject();
  //      //controlRenderer->setTexture(texture);
  //      //controlRenderer->setColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 0.4f));
		//controlRenderer->setIsGlassy(true);

  //      textRenderer = label1->getComponent<phi::textRenderer>();
  //      textRenderer->setFont(font);
  //      textRenderer->setColor(color::fromRGBA(1.0f, 1.f, 1.0f, 1.0f));
  //      _ui->add(label1);
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
        debug("fps: " + std::to_string(application::framesPerSecond));
#if _DEBUG
        _gl->shadersManager->reloadAllShaders();
#endif
    }

    void screen::onClosing()
    {
        safeDelete(_commandsManager);
        safeDelete(_defaultController);
        safeDelete(_gl);
        safeDelete(_library);
        safeDelete(_scene);
        safeDelete(_ui);
    }
}