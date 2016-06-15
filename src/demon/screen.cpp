#include <precompiled.h>
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

#ifdef _DEBUG
#include <rendering\liveShaderReloader.h>
#endif

using namespace phi;

namespace demon
{
    screen::screen(wstring title, resolution resolution) :
        window(title, resolution),
        _activeContext(nullptr)
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
        //framebuffer::createPickingFramebuffer(_resolution);

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
        auto font = fontsManager::load("Roboto-Thin.ttf", 8);
        auto fontFps = fontsManager::load("Roboto-Thin.ttf", 12);

        auto labelNandinho = labelBuilder::newLabel(L"nanddiiiiiiiinho layer says hello!1")
            .withPosition(vec3(-100.f, 0.f, 0.f))
            .withControlColor(.4f, .7f, .9f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        _labelFps = labelBuilder::newLabel(L"press ctrl + space to change context bro \\m/")
            .withPosition(vec3(-200.f, 100.f, 0.f))
            .withControlColor(.7f, .5f, .9f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(fontFps)
            .build();

        auto constructionLabel = labelBuilder::newLabel(L"construction layer says hello!1")
            .withPosition(vec3(-200.f, 100.f, 0.f))
            .withControlColor(.9f, .6f, .9f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        auto sceneLabel = labelBuilder::newLabel(L"o")
            .withPosition(vec3(0.f, 0.f, 0.f))
            .withControlColor(.9f, .6f, .9f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        auto floor = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        auto chair = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair->getTransform()->setLocalPosition(vec3(0.f, .1f, .0f));

        auto sceneCamera = new camera(_resolution, 0.1f, 10000.0f, PI_OVER_4);
        sceneCamera->getTransform()->setLocalPosition(vec3(-5.0f, 5.0f, 20.0f));
        sceneCamera->getTransform()->setDirection(-vec3(-5.0f, 5.0f, 20.0f));

        _framebufferAllocator = new framebufferAllocator();

        _sceneLayer = layerBuilder::newLayer(sceneCamera, application::resourcesPath, _framebufferAllocator)
            .withMeshRenderer()
            .build();

        auto constructionCamera = new camera(_resolution, 0.1f, 10000.0f, PI_OVER_4);
        constructionCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
        constructionCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

        _constructionLayer = layerBuilder::newLayer(constructionCamera, application::resourcesPath, _framebufferAllocator)
            .withControlRenderer()
            .withTextRenderer()
            .build();

        auto nandinhoCamera = new camera(_resolution, 0.1f, 10000.0f, PI_OVER_4);
        nandinhoCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
        nandinhoCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

        _nandinhoLayer = layerBuilder::newLayer(nandinhoCamera, application::resourcesPath, _framebufferAllocator)
            .withGlassyControlRenderer()
            .withTextRenderer()
            .build();

        //TODO: onDemandUI
        //sceneLayer->addOnNodeAdded([=](node* node)
        //{
            //node->addOnClick([=](layer* layer)
            //{
                //auto onDemandBillboard = onDemandBillboardUI::createFrom(node, sceneCamera);
                //auto onDemand = onDemandUI::createFrom(node);
                //layer->add(onDemand);
            //});
        //});

        _framebufferAllocator->allocate(_resolution);

        _designContext = new context(_resolution, { _sceneLayer });
        _constructionContext = new context(_resolution, { _sceneLayer });

        _sceneLayer->add(floor);
        _sceneLayer->add(chair); 
        //sceneLayer->add(sceneLabel);
        //TODO: prevent components that are not dealt with it from being added to layer

        _constructionLayer->add(constructionLabel);

        _nandinhoLayer->add(labelNandinho);
        _nandinhoLayer->add(_labelFps);

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

        //safeDelete(_sceneLayer);
        safeDelete(_constructionLayer);
        safeDelete(_nandinhoLayer);

        safeDelete(_designContext);
        safeDelete(_constructionContext);

#ifdef _DEBUG
        _watcher->endWatch();
        safeDelete(_watcher);
        safeDelete(_messageQueue);
#endif 
    }

    void screen::onResize(phi::resolution resolution)
    {
        gl::resize(resolution);
        //glViewport(0, 0, (GLsizei)(resolution.width / 2.0f), (GLsizei)(resolution.height / 2.0f));

        if (_activeContext)
            _activeContext->resize(resolution);

        debug(resolution.toString());
    }
}