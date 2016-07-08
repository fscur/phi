#include <precompiled.h>
#include "screen.h"

#include <diagnostic\stopwatch.h>

#include <core\multiCommand.h>

#include <loader\importer.h>

#include <animation\floatAnimator.h>

#include <rendering\pickingFramebuffer.h>
#ifdef _DEBUG
#include <rendering\liveShaderReloader.h>
#endif

#include <ui\labelBuilder.h>
#include <ui\buttonBuilder.h>
#include <ui\control.h>
#include <ui\text.h>

#include <application\application.h>
#include <application\undoCommand.h>
#include <application\redoCommand.h>

#include <context\layerBuilder.h>
#include <context\deleteSceneObjectCommand.h>
#include <core\clickComponent.h>

using namespace phi;

namespace demon
{
    screen::screen(wstring title, resolution resolution) :
        window(title, resolution),
        _activeContext(nullptr),
        _framebufferAllocator(nullptr)
    {
    }

    screen::~screen()
    {
    }

    void screen::onInit()
    {
        initGL();
        initPickingFramebuffer();
        initLibraries();
        initScene();
        initInput();
        initUi();

#ifdef _DEBUG
        _messageQueue = new blockingQueue<phi::watcherMessage>();
        auto shadersPath = path::combine(application::resourcesPath, "shaders");
        _watcher = new watcher(shadersPath, _messageQueue, [&](fileInfo shaderFileInfo)
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

        _framebufferAllocator = new framebufferAllocator();
    }

    void screen::initPickingFramebuffer()
    {
        pickingFramebuffer::initialize(_framebufferAllocator, _resolution);
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

    bool _design = true;
    class changeContextCommand :
        public command
    {
    public:
        virtual void execute() override { _design = !_design; }
        virtual void executeUndo() override { _design = !_design; }
    };

    phi::node* chair0;
    phi::node* chair1;
    phi::node* chair2;
    phi::node* chair3;
    phi::node* cube0;
    phi::node* cube1;
    phi::node* cube2;
    phi::node* floor0;
    phi::node* floor1;

    void screen::initUi()
    {
        auto font = fontsManager::load("Roboto-Thin.ttf", 10);
        auto fontFps = fontsManager::load("Roboto-Thin.ttf", 12);

        _labelNandinho = labelBuilder::newLabel(L"ab")
            .withPosition(vec3(-100.f, 50.f, 0.f))
            .withControlColor(1.0f, 0.0f, 0.0f, 1.0f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        _labelFps = labelBuilder::newLabel(L"abc")
            .withPosition(vec3(-200.f, 100.f, 0.f))
            .withControlColor(1.0f, 0.0f, 1.0f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(fontFps)
            .build();

        _constructionLabel = labelBuilder::newLabel(L"abcd")
            .withPosition(vec3(200.f, 50.f, 0.f))
            .withControlColor(0.0f, 1.0f, 0.5f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        _sceneLabel = labelBuilder::newLabel(L"abcde")
            .withPosition(vec3(100.f, 100.f, 0.f))
            .withControlColor(1.0f, 0.0f, 0.5f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        auto changeContextButton = buttonBuilder::newButton()
            .withPosition(vec3(-200.f, -20.f, 0.f))
            .withText(L"Change context")
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .withControlColor(.5f, .5f, .2f, 1.f)
            .withAction([=](node* node)
            {
                _commandsManager->executeCommand(new changeContextCommand());
            })
            .build();

        chair0 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair0->getTransform()->setLocalPosition(vec3(4.f, .1f, -2.0f));

        chair1 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair1->getTransform()->setLocalPosition(vec3(2.f, .1f, .0f));

        chair2 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair2->getTransform()->setLocalPosition(vec3(0.f, .1f, 2.f));

        chair3 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair3->getTransform()->setLocalPosition(vec3(2.f, .1f, 2.f));

        cube0 = _userLibrary->getObjectsRepository()->getAllResources()[1]->getClonedObject();

        cube1 = _userLibrary->getObjectsRepository()->getAllResources()[1]->getClonedObject();
        cube1->getTransform()->setLocalPosition(vec3(-50.f, .1f, 2.f));

        cube2 = _userLibrary->getObjectsRepository()->getAllResources()[1]->getClonedObject();
        cube2->getTransform()->setLocalPosition(vec3(2.f, .1f, -2.f));

        floor0 = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();

        floor1 = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        floor1->getTransform()->setLocalPosition(vec3(0.f, 0.f, 10.f));

        auto sceneCamera = new camera(_resolution, 0.1f, 1000.0f, PI_OVER_4);
        sceneCamera->getTransform()->setLocalPosition(vec3(-5.0f, 5.0f, 20.0f));
        sceneCamera->getTransform()->setDirection(-vec3(-5.0f, 5.0f, 20.0f));

        _sceneLayer = layerBuilder::newLayer(sceneCamera, application::resourcesPath, _framebufferAllocator, _commandsManager)
            .withMeshRenderer()
            .build();

        auto constructionCamera = new camera(_resolution, 0.1f, 1000.0f, PI_OVER_4);
        constructionCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
        constructionCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

        _constructionLayer = layerBuilder::newLayer(constructionCamera, application::resourcesPath, _framebufferAllocator, _commandsManager)
            .withControlRenderer()
            //.withTextRenderer()
            .build();

        auto nandinhoCamera = new camera(_resolution, 0.1f, 1000.0f, PI_OVER_4);
        nandinhoCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
        nandinhoCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

        _nandinhoLayer = layerBuilder::newLayer(nandinhoCamera, application::resourcesPath, _framebufferAllocator, _commandsManager)
            .withGlassyControlRenderer()
            .withTextRenderer()
            .build();

        //TODO: onDemandUI
        //sceneLayer->addOnNodeAdded([=](node* node)
        //{
            //node->addOnClick([=](layer* layer)
            //{
                //auto onDemandBillboard = onDemandBillboardUI::from(node, sceneCamera);
                //auto onDemand = onDemandUI::from(node);
                //layer->add(onDemand);
            //});
        //});

        _framebufferAllocator->allocate(_resolution);

        _designContext = new context(
            _resolution,
            _framebufferAllocator,
            _commandsManager,
            { _sceneLayer, _nandinhoLayer });

        _constructionContext = new context(
            _resolution,
            _framebufferAllocator,
            _commandsManager,
            { _sceneLayer, _constructionLayer });

        _sceneLayer->add(cube0);
        _sceneLayer->add(cube1);
        _sceneLayer->add(cube2);
        _sceneLayer->add(chair0);
        _sceneLayer->add(chair1);
        _sceneLayer->add(chair2);
        _sceneLayer->add(chair3);
        _sceneLayer->add(floor0);
        _sceneLayer->add(floor1);

        //_sceneLayer->add(_sceneLabel);
        //TODO: prevent components that are not dealt with it from being added to layer

        _constructionLayer->add(_constructionLabel);
        _nandinhoLayer->add(changeContextButton);

        _nandinhoLayer->add(_sceneLabel);
        _nandinhoLayer->add(_constructionLabel);
        _nandinhoLayer->add(_labelNandinho);
        _nandinhoLayer->add(_labelFps);
        _nandinhoLayer->add(changeContextButton);

        _activeContext = _designContext;

        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_DELETE }, [&]()
        {
            auto nodesToDelete = { cube0 };
            return new deleteSceneObjectCommand(nodesToDelete);
        }));

        _commandsManager->addShortcut(shortcut({ PHIK_SHIFT, PHIK_DELETE }, [&]()
        {
            auto nodesToDelete = { cube1 };
            return new deleteSceneObjectCommand(nodesToDelete);
        }));

        _commandsManager->addShortcut(shortcut({ PHIK_SPACE }, [&]()
        {
            auto nodesToDelete = { cube2 };
            return new deleteSceneObjectCommand(nodesToDelete);
        }));

        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_0 }, [&]()
        {
            auto nodesToDelete = { chair0 };
            return new deleteSceneObjectCommand(nodesToDelete);
        }));

        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_1 }, [&]()
        {
            auto nodesToDelete = { chair1 };
            return new deleteSceneObjectCommand(nodesToDelete);
        }));

        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_2 }, [&]()
        {
            auto nodesToDelete = { chair2 };
            return new deleteSceneObjectCommand(nodesToDelete);
        }));

        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_3 }, [&]()
        {
            auto nodesToDelete = { chair3 };
            return new deleteSceneObjectCommand(nodesToDelete);
        }));

        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_4 }, [&]()
        {
            auto nodesToDelete = { floor0 };
            return new deleteSceneObjectCommand(nodesToDelete);
        }));

        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_5 }, [&]()
        {
            auto nodesToDelete = { floor1 };
            return new deleteSceneObjectCommand(nodesToDelete);
        }));
    }

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

    bool a = false;

    void screen::onTick()
    {
        auto label = _labelFps->getComponent<phi::text>();
        auto str = "fps: " + std::to_string(application::framesPerSecond);
        debug(str);
        label->setText(wstring(str.begin(), str.end()));

        if (a)
        {
            _constructionLabel->getComponent<phi::text>()->setText(L"edftg");
            _nandinhoLayer->add(_labelFps);
            _nandinhoLayer->add(_sceneLabel);
        }
        else
        {
            _labelFps->getParent()->removeChild(_labelFps);
            _sceneLabel->getParent()->removeChild(_sceneLabel);
        }

        a = !a;

#ifdef _DEBUG
        while (!_messageQueue->empty())
        {
            auto message = _messageQueue->front();
            message.callback(message.fileChanged);
            _messageQueue->pop();
        }
#endif
    }

    void screen::onSwapBuffers()
    {
        gl::syncPipeline();
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
        window::onResize(resolution);
        gl::resize(resolution);
        pickingFramebuffer::resize(resolution);

        _framebufferAllocator->reallocate(resolution);
        _activeContext->resize(resolution);
    }
}