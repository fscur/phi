﻿#include <precompiled.h>
#include "screen.h"

#include <diagnostic\stopwatch.h>

#include <core\multiCommand.h>

#include <loader\importer.h>

#include <animation\floatAnimator.h>

#include <context\unselectSceneObjectCommand.h>
#include <context\groupSceneObjectsCommand.h>
#include <context\deleteSceneObjectCommand.h>

#include <application\application.h>
#include <application\undoCommand.h>
#include <application\redoCommand.h>

#include <ui\labelBuilder.h>
#include <ui\control.h>
#include <ui\text.h>

#include "addObjectCommand.h"

#include <rendering\renderPass.h>
#include <rendering\shader.h>

#include <context\layerBuilder.h>
#include <context\contextBuilder.h>

#ifdef _DEBUG
#include <rendering\liveShaderReloader.h>
#endif

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
        _watcher = new watcher(application::resourcesPath + "/shaders", _messageQueue, [&](fileInfo shaderFileInfo)
        {
            liveShaderReloader::reloadShader(shaderFileInfo.path);
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
    }

    void screen::initUi()
    {
        auto font = _gl->fontsManager->load("Roboto-Thin.ttf", 8);
        auto fontFps = _gl->fontsManager->load("Roboto-Thin.ttf", 12);

        auto labelNandinho = labelBuilder::newLabel(L"nanddiiiiiiiinho layer says hello!1", _gl)
            .withPosition(vec3(-100.f, 0.f, 0.f))
            .withControlColor(.4f, .7f, .9f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        _labelFps = labelBuilder::newLabel(L"press ctrl + space to change context bro \\m/", _gl)
            .withPosition(vec3(-200.f, 100.f, 0.f))
            .withControlColor(.7f, .5f, .9f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(fontFps)
            .build();

        auto constructionLabel = labelBuilder::newLabel(L"construction layer says hello!1", _gl)
            .withPosition(vec3(-200.f, 100.f, 0.f))
            .withControlColor(.9f, .6f, .9f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        auto sceneLabel = labelBuilder::newLabel(L"o", _gl)
            .withPosition(vec3(0.f, 0.f, 0.f))
            .withControlColor(.9f, .6f, .9f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        auto floor = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        auto chair = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair->getTransform()->setLocalPosition(vec3(0.f, .1f, .0f));

        auto resolution = phi::resolution(static_cast<float>(_width), static_cast<float>(_height));
        auto sceneCamera = new camera("sceneCamera", resolution, 0.1f, 10000.0f, PI_OVER_4);
        auto sceneLayer = layerBuilder::newLayer(sceneCamera, _gl, resolution, application::resourcesPath)
            .withMeshRenderer()
            .build();

        auto constructionCamera = new camera("constructionCamera", resolution, 0.1f, 10000.0f, PI_OVER_4);
        auto constructionLayer = layerBuilder::newLayer(constructionCamera, _gl, resolution, application::resourcesPath)
            .withControlRenderer()
            .withTextRenderer()
            .build();

        auto nandinhoCamera = new camera("nandinhoCamera", resolution, 0.1f, 10000.0f, PI_OVER_4);
        auto nandinhoLayer = layerBuilder::newLayer(nandinhoCamera, _gl, resolution, application::resourcesPath)
            .withGlassyControlRenderer()
            .withTextRenderer()
            .build();

        _designContext = new context({ sceneLayer, nandinhoLayer });
        _constructionContext = new context({ sceneLayer, constructionLayer });

        sceneLayer->add(floor);
        sceneLayer->add(chair);
        sceneLayer->add(sceneLabel);

        constructionLayer->add(constructionLabel);

        nandinhoLayer->add(labelNandinho);
        nandinhoLayer->add(_labelFps);

        // GAMBIS DAR UM JEITO!!!!!!!!
        auto cameraPosition = vec3(-5.0f, 5.0f, 20.0f);
        sceneCamera->getTransform()->setLocalPosition(cameraPosition);
        sceneCamera->getTransform()->setDirection(-cameraPosition);

        constructionCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
        constructionCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

        nandinhoCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
        nandinhoCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

        _activeContext = _designContext;
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
        input::mouseDown->assign(std::bind(&screen::onMouseDown, this, std::placeholders::_1));
        input::mouseMove->assign(std::bind(&screen::onMouseMove, this, std::placeholders::_1));
        input::mouseUp->assign(std::bind(&screen::onMouseUp, this, std::placeholders::_1));
        input::mouseWheel->assign(std::bind(&screen::onMouseWheel, this, std::placeholders::_1));
        input::keyDown->assign(std::bind(&screen::onKeyDown, this, std::placeholders::_1));
        input::keyUp->assign(std::bind(&screen::onKeyUp, this, std::placeholders::_1));

        _commandsManager = new commandsManager();
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_z }, [&]() { return new undoCommand(_commandsManager); }));
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_y }, [&]() { return new redoCommand(_commandsManager); }));
        //_commandsManager->addShortcut(shortcut({ PHIK_DELETE }, [&]()
        //{
        //    return new multiCommand(vector<command*>
        //    {
        //        new unselectSceneObjectCommand(_scene->getSelectedObjects()),
        //        new deleteSceneObjectCommand(_scene->getSelectedObjects())
        //    });
        //}));

        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_SPACE }, [&]() { return new changeContextCommand(); }));
    }

    void screen::onMouseDown(phi::mouseEventArgs* e)
    {
        _activeContext->onMouseDown(e);
    }

    void screen::onMouseMove(phi::mouseEventArgs* e)
    {
        _activeContext->onMouseMove(e);
    }

    void screen::onMouseUp(phi::mouseEventArgs* e)
    {
        _activeContext->onMouseUp(e);
    }

    void screen::onMouseWheel(phi::mouseEventArgs* e)
    {
        _activeContext->onMouseWheel(e);
    }

    void screen::onKeyDown(phi::keyboardEventArgs* e)
    {
        _activeContext->onKeyDown(e);
    }

    void screen::onKeyUp(phi::keyboardEventArgs* e)
    {
        _activeContext->onKeyUp(e);
    }

    void screen::onUpdate()
    {
        if (_design)
            _activeContext = _designContext;
        else
            _activeContext = _constructionContext;

        phi::floatAnimator::update();

        _activeContext->update();
    }

    void screen::onRender()
    {
        _activeContext->render();
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
        safeDelete(_gl);
        safeDelete(_userLibrary);
        safeDelete(_projectLibrary);
#ifdef _DEBUG
        _watcher->endWatch();
        safeDelete(_watcher);
        safeDelete(_messageQueue);
#endif 
    }
}