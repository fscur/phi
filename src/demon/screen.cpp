#include <precompiled.h>
#include "screen.h"

#include <diagnostics\stopwatch.h>

#include <loader\importer.h>

#include <core\model.h>

#include <animation\floatAnimator.h>

#include <apps\application.h>
#include <apps\undoCommand.h>
#include <apps\redoCommand.h>

#include <ui\control.h>
#include <ui\text.h>

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
        initLibraries();
        initScene();
        initUi();
        initInput();

#ifdef _DEBUG
        _messageQueue = new blockingQueue<phi::watcherMessage>();
        _watcher = new watcher(application::resourcesPath + "/shaders", _messageQueue, [&](string shaderFileName)
        {
            auto fileExtension = path::getExtension(shaderFileName);
            if (fileExtension == phi::shadersManager::FRAG_EXT ||
                fileExtension == phi::shadersManager::VERT_EXT)
            {
                auto shaderName = path::getFileNameWithoutExtension(shaderFileName);
                _gl->shadersManager->reloadShader(shaderName);
            }
        });
        _watcher->startWatch();
#endif
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

    void screen::initLibraries()
    {
        _userLibrary = new library(application::libraryPath);
        _userLibrary->load();

        _projectLibrary = new library(application::path);
    }

    void screen::initScene()
    {
        _scene = new scene(_gl, static_cast<float>(_width), static_cast<float>(_height));
        auto camera = _scene->getCamera();

        auto cameraTransform = camera->getTransform();
        auto cameraPos = vec3(0.0f, 0.0f, 10.0f);
        cameraTransform->setLocalPosition(cameraPos);
        cameraTransform->setDirection(-cameraPos);

        auto floor = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        _scene->add(floor);

        auto chair = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair->getTransform()->setLocalPosition(vec3(0.f, .5f, 0.f));
        _scene->add(chair);
    }

    void screen::initUi()
    {
        camera* uiCamera = new camera("uiCamera", static_cast<float>(_width), static_cast<float>(_height), 0.1f, 10000.0f, PI_OVER_4);

        _ui = new ui(uiCamera, _scene->getRenderer(), _gl, static_cast<float>(_width), static_cast<float>(_height));

        auto font = _gl->fontsManager->load("Roboto-Thin.ttf", 24);

        auto label0 = _ui->newLabel(L"nanddiiiiiiiinho", vec3(-100.0f, 0.0f, 0.0f));
        auto control = label0->getComponent<phi::control>();
        control->setColor(color::fromRGBA(0.9f, 0.9f, 0.9f, 1.0f));
        control->setIsGlassy(true);

        auto text = label0->getComponent<phi::text>();
        text->setFont(font);
        text->setColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

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
#ifdef _DEBUG
        while (!_messageQueue->empty())
        {
            auto message = _messageQueue->front();
            message.callback(message.fileChanged);
            _messageQueue->pop();
        }
#endif
    }

    void screen::onClosing()
    {
        safeDelete(_commandsManager);
        safeDelete(_defaultController);
        safeDelete(_gl);
        safeDelete(_userLibrary);
        safeDelete(_projectLibrary);
        safeDelete(_scene);
        safeDelete(_ui);
#ifdef _DEBUG
        _watcher->endWatch();
        safeDelete(_watcher);
        safeDelete(_messageQueue);
#endif 
    }
}