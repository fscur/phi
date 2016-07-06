#include <precompiled.h>
#include "screen.h"

#include <diagnostics\stopwatch.h>

#include <core\multiCommand.h>

#include <loader\importer.h>

#include <animation\floatAnimator.h>

#include <scenes\unselectSceneObjectCommand.h>
#include <scenes\groupSceneObjectsCommand.h>
#include <scenes\deleteSceneObjectCommand.h>

#include <apps\application.h>
#include <apps\undoCommand.h>
#include <apps\redoCommand.h>

#include <ui\control.h>
#include <ui\text.h>

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
        //floor->getTransform()->yaw(PI_OVER_4);
        _scene->add(floor);

        auto floor2 = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        floor2->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, -2.5f));
        floor2->getTransform()->pitch(PI_OVER_2);
        _scene->add(floor2);

        auto chair0 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair0->getTransform()->yaw(PI_OVER_4);
        chair0->getTransform()->setLocalPosition(vec3(0.f, .5f, .0f));

        auto chair1 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair1->getTransform()->setLocalPosition(vec3(2.0f, .5f, 0.f));

        auto chair2 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair2->getTransform()->setLocalPosition(vec3(0.f, .5f, 2.0f));

        auto chair3 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair3->getTransform()->setLocalPosition(vec3(2.0f, .5f, 2.0f));

        auto cube = _userLibrary->getObjectsRepository()->getAllResources()[1]->getClonedObject();
        cube->getTransform()->setLocalPosition(vec3(-2.0f, 0.7f, 0.0f));

        _scene->add(chair0);
        _scene->add(chair1);
        _scene->add(chair2);
        _scene->add(chair3);
        _scene->add(cube);
    }

    void screen::initUi()
    {
        camera* uiCamera = new camera("uiCamera", static_cast<float>(_width), static_cast<float>(_height), 0.1f, 10000.0f, PI_OVER_4);

        _ui = new ui(uiCamera, _scene->getRenderer(), _gl, static_cast<float>(_width), static_cast<float>(_height));

        //auto font = _gl->fontsManager->load("Roboto-Thin.ttf", 24);
        //auto fontFps = _gl->fontsManager->load("Roboto-Thin.ttf", 12);

        //auto labelNandinho = _ui->newLabel(L"nanddiiiiiiiinho", vec3(-100.0f, 0.0f, 0.0f));
        //auto control = labelNandinho->getComponent<phi::control>();
        //control->setColor(color::fromRGBA(0.9f, 0.9f, 0.9f, 1.0f));
        //control->setIsGlassy(true);

        //auto text = labelNandinho->getComponent<phi::text>();
        //text->setFont(font);
        //text->setColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

        //_labelFps = _ui->newLabel(L"Fps: ", vec3(-200.f, 100.f, 0.f));
        //auto fpsControl = _labelFps->getComponent<phi::control>();
        //fpsControl->setColor(color::fromRGBA(.7f, .5f, .9f, 1.0f));
        //fpsControl->setIsGlassy(true);

        //auto textFps = _labelFps->getComponent<phi::text>();
        //textFps->setFont(fontFps);
        //textFps->setColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

        //_ui->add(labelNandinho);
        //_ui->add(_labelFps);
    }

    void screen::initInput()
    {
        _commandsManager = new commandsManager();
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_z }, [&]() { return new undoCommand(_commandsManager); }));
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_y }, [&]() { return new redoCommand(_commandsManager); }));
        _commandsManager->addShortcut(shortcut({ PHIK_DELETE }, [&]()
        {
            return new multiCommand(vector<command*>
            {
                new unselectSceneObjectCommand(_scene->getSelectedObjects()),
                new deleteSceneObjectCommand(_scene->getSelectedObjects())
            });
        }));

        _defaultController = new defaultCameraController(_scene, _commandsManager);
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
        //auto label = _labelFps->getComponent<phi::text>();
        //auto str = "fps: " + std::to_string(application::framesPerSecond);

        //label->setText(wstring(str.begin(), str.end()));

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