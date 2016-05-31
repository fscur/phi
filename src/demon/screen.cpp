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

#include <rendering\renderPass.h>
#include <rendering\shader.h>

#include "layerBuilder.h"
#include "contextBuilder.h"

using namespace phi;

namespace demon
{
    screen::screen(string name, uint width, uint height) :
        window(name, width, height)
    {
    }

    screen::~screen()
    {
        //safeDelete(_designContext);
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
            /*auto fileExtension = path::getExtension(shaderFileName);
            if (fileExtension == phi::shadersManager::FRAG_EXT ||
                fileExtension == phi::shadersManager::VERT_EXT)
            {
                auto shaderName = path::getFileNameWithoutExtension(shaderFileName);
                _gl->shadersManager->reloadShader(shaderName);
            }*/
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
    }

    void screen::initUi()
    {
        camera* uiCamera = new camera("uiCamera", static_cast<float>(_width), static_cast<float>(_height), 0.1f, 10000.0f, PI_OVER_4);

        _ui = new ui(uiCamera, _gl, static_cast<float>(_width), static_cast<float>(_height));

        auto font = _gl->fontsManager->load("Roboto-Thin.ttf", 14);
        auto fontFps = _gl->fontsManager->load("Roboto-Thin.ttf", 12);

        auto labelNandinho = _ui->newLabel(L"nanddiiiiiiiinho layer says hello!1", vec3(-100.0f, 0.0f, 0.0f));
        auto control = labelNandinho->getComponent<phi::control>();
        control->setColor(color::fromRGBA(0.4f, 0.7f, 0.9f, 1.0f));
        auto text = labelNandinho->getComponent<phi::text>();
        text->setFont(font);
        text->setColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

        _labelFps = _ui->newLabel(L"press ctrl + space to change context bro \\m/", vec3(-200.f, 100.f, 0.f));
        auto fpsControl = _labelFps->getComponent<phi::control>();
        fpsControl->setColor(color::fromRGBA(.7f, .5f, .9f, 1.0f));
        auto textFps = _labelFps->getComponent<phi::text>();
        textFps->setFont(fontFps);
        textFps->setColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

        auto constructionLabel = _ui->newLabel(L"construction layer says hello!1", vec3(-200.f, 100.f, 0.f));
        auto constructionControl = constructionLabel->getComponent<phi::control>();
        constructionControl->setColor(color::fromRGBA(.9f, .6f, .9f, 1.0f));
        auto construction = constructionLabel->getComponent<phi::text>();
        construction->setFont(font);
        construction->setColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

        auto floor = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        auto chair = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair->getTransform()->setLocalPosition(vec3(0.f, .1f, .0f));

        auto sceneLayer = layerBuilder::buildScene(application::resourcesPath, _gl, (float) _width, (float)_height);
        sceneLayer->add(floor);
        sceneLayer->add(chair);

        auto nandinhoLayer = layerBuilder::buildUI(application::resourcesPath, _gl, (float) _width/5, (float)_height/5);
        nandinhoLayer->add(labelNandinho);
        nandinhoLayer->add(_labelFps);

        auto constructionLayer = layerBuilder::buildUI(application::resourcesPath, _gl, (float)_width, float(_height));
        constructionLayer->add(constructionLabel);

        _designContext = new context({ sceneLayer, nandinhoLayer });
        //_constructionContext = new context({ sceneLayer, constructionLayer });

        //_designContext = new context({ sceneLayer});
        _constructionContext = new context({ sceneLayer });
    }

    bool _design = true;
    class changeContextCommand :
        public command
    {
    public:
        virtual void execute() override { _design = !_design; }
        virtual void executeUndo() override { _design = !_design; }
    };

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

        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_SPACE }, [&]() { return new changeContextCommand(); }));
        _defaultController = new defaultCameraController(_scene, _commandsManager);
    }

    void screen::onUpdate()
    {
        _scene->update();

        phi::floatAnimator::update();
        _defaultController->update();

        if(_design)
            _designContext->update();
        else
            _constructionContext->update();
    }

    void screen::onRender()
    {
        if(_design)
            _designContext->render();
        else
            _constructionContext->render();
    }

    void screen::onTick()
    {
        auto label = _labelFps->getComponent<phi::text>();
        auto str = "fps: " + std::to_string(application::framesPerSecond);
        debug(str);
        label->setText(wstring(str.begin(), str.end()));

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