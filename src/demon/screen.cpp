#include <precompiled.h>
#include "screen.h"
#include "changeContextCommand.h"
#include <diagnostic\stopwatch.h>
#include <core\multiCommand.h>
#include <loader\importer.h>
#include <rendering\defaultFramebuffer.h>
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
#include <context\invalidLayerConfigurationException.h>

#include <core\input.h>
#include <core\clickComponent.h>
#include <core\boxCollider.h>
#include <ui\planeGrid.h>
#include <core\ghostMesh.h>

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
        initFramebuffers();
        initLibraries();
        initInput();
        initContexts();
        initWatcher();
    }

    void screen::initWatcher()
    {
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

    void screen::initFramebuffers()
    {
        defaultFramebuffer::initialize(_framebufferAllocator, _resolution);
        pickingFramebuffer::initialize(_framebufferAllocator, _resolution);
    }

    void screen::initLibraries()
    {
        _userLibrary = new library(application::libraryPath);
        _userLibrary->load();

        _projectLibrary = new library(application::path);
    }

    void screen::initContexts()
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
            .withControlColor(0.0f, 1.0f, 0.5, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        auto changeContextButton = buttonBuilder::newButton()
            .withPosition(vec3(-200.f, -20.f, 0.f))
            .withText(L"Change context")
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .withControlColor(.5, .5, .2f, 1.f)
            .withAction([=](node* node)
            {
                _commandsManager->executeCommand(new changeContextCommand());
            })
            .build();

        _chair0 = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        _chair0->getTransform()->setLocalPosition(vec3(4.f, 0.0f, -2.0f));

        auto cube0 = _userLibrary->getObjectsRepository()->getAllResources()[7]->getClonedObject();
        //cube0->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 0.0f));
        auto cube1 = _userLibrary->getObjectsRepository()->getAllResources()[7]->getClonedObject();
        cube1->getTransform()->setLocalPosition(vec3(0.5f, 1.5f, 0.0f));
        auto group = new node();
        group->addChild(cube0);
        group->addChild(cube1);

        //cube0->getTransform()->yaw(PI_OVER_4);

        auto back_wall = _userLibrary->getObjectsRepository()->getAllResources()[21]->getClonedObject();
        back_wall->getTransform()->setLocalPosition(vec3(0.0f, DECIMAL_TRUNCATION, -2.4f));
        //cube1->getTransform()->translate(vec3(2.0f, 0.5f, 0.0f));
        //cube1->getTransform()->yaw(PI_OVER_4);
        auto floor0 = _userLibrary->getObjectsRepository()->getAllResources()[24]->getClonedObject();
        //floor0->getTransform()->setLocalSize(vec3(1.0f, 2.0f, 1.0f));
        //auto wall = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        //wall->getTransform()->pitch(PI_OVER_2);
        //wall->getTransform()->setLocalPosition(vec3(0.0f, 2.5f, -2.5f));

        auto coffeTable = _userLibrary->getObjectsRepository()->getAllResources()[29]->getClonedObject();
        coffeTable->getTransform()->translate(vec3(2.0f, 0.0f, 0.0f));
        auto tableChair = _userLibrary->getObjectsRepository()->getAllResources()[5]->getClonedObject();
        tableChair->getTransform()->translate(vec3(-2.0f, 0.0f, 0.0f));
        auto table = _userLibrary->getObjectsRepository()->getAllResources()[28]->getClonedObject();
        table->getTransform()->translate(vec3(4.0f, 0.0f, 0.0f));

        _sceneCamera = new camera(_resolution, 0.1f, 1000.0f, PI_OVER_4);
        _sceneCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.5f, 2.0f));
        _sceneCamera->getTransform()->yaw(PI);

        try
        {
            _sceneLayer = layerBuilder::newLayer(_sceneCamera, application::resourcesPath, _framebufferAllocator, _commandsManager)
                .withMeshRenderer()
                .withGhostMeshRenderer()
                //.withBoxColliderRenderer()
                .withPlaneGridRenderer()
                .withPhysics()
                .withAnimation()
                .withCameraController()
                .withSelectionController()
                .withTranslationController()
                .build();

            _constructionCamera = new camera(_resolution, 0.1f, 1000.0f, PI_OVER_4);
            _constructionCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
            _constructionCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

            _constructionLayer = layerBuilder::newLayer(_constructionCamera, application::resourcesPath, _framebufferAllocator, _commandsManager)
                .withControlRenderer()
                .withTextRenderer()
                .build();

            _nandinhoCamera = new camera(_resolution, 0.1f, 1000.0f, PI_OVER_4);
            _nandinhoCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
            _nandinhoCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

            _nandinhoLayer = layerBuilder::newLayer(_nandinhoCamera, application::resourcesPath, _framebufferAllocator, _commandsManager)
                .withGlassyControlRenderer()
                .withTextRenderer()
                .build();
        }
        catch (phi::invalidLayerConfigurationException& ex)
        {
            phi::application::logError(ex.what());
        }

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

        //_sceneLayer->add(cube0);
        //_sceneLayer->add(cube1);
        _sceneLayer->add(group);
        _sceneLayer->add(_chair0);
        //_sceneLayer->add(floor0);
        //_sceneLayer->add(back_wall);
        //_sceneLayer->add(table);
        //_sceneLayer->add(tableChair);
        //_sceneLayer->add(coffeTable);

        //TODO: prevent components that are not dealt with it from being added to layer

        _constructionLayer->add(_constructionLabel);
        _nandinhoLayer->add(_labelNandinho);
        _nandinhoLayer->add(_labelFps);
        _nandinhoLayer->add(changeContextButton);

        _activeContext = _designContext;

        //_commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_DELETE }, [=]()
        //{
        //    auto nodesToDelete = { cube0 };
        //    return new deleteSceneObjectCommand(nodesToDelete);
        //}));

        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_0 }, [=]()
        {
            auto nodesToDelete = { _chair0 };
            return new deleteSceneObjectCommand(nodesToDelete);
        }));

        //_commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_4 }, [=]()
        //{
        //    auto nodesToDelete = { floor0 };
        //    return new deleteSceneObjectCommand(nodesToDelete);
        //}));
    }

    void screen::initInput()
    {
        input::mouseDown->assign(std::bind(&screen::onMouseDown, this, std::placeholders::_1));
        input::mouseMove->assign(std::bind(&screen::onMouseMove, this, std::placeholders::_1));
        input::mouseUp->assign(std::bind(&screen::onMouseUp, this, std::placeholders::_1));
        input::beginMouseWheel->assign(std::bind(&screen::onBeginMouseWheel, this, std::placeholders::_1));
        input::mouseWheel->assign(std::bind(&screen::onMouseWheel, this, std::placeholders::_1));
        input::endMouseWheel->assign(std::bind(&screen::onEndMouseWheel, this, std::placeholders::_1));
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

    void screen::onBeginMouseWheel(phi::mouseEventArgs* e)
    {
        _activeContext->onBeginMouseWheel(e);
    }

    void screen::onMouseWheel(phi::mouseEventArgs* e)
    {
        _activeContext->onMouseWheel(e);
    }

    void screen::onEndMouseWheel(phi::mouseEventArgs* e)
    {
        _activeContext->onEndMouseWheel(e);
    }

    void screen::onKeyDown(phi::keyboardEventArgs* e)
    {
        _activeContext->onKeyDown(e);
    }

    void screen::onKeyUp(phi::keyboardEventArgs* e)
    {
        _activeContext->onKeyUp(e);
    }

    float t = 0;

    void screen::onUpdate()
    {
        t += 0.01f;
        vec3 pos = vec3(glm::cos(t) * 3.2f, glm::sin(t) * 3.2f, 0.0f);
        //_planeNode1->getTransform()->roll(0.01f);
        //_planeNode1->getTransform()->setLocalPosition(pos);

        if (_design)
            _activeContext = _designContext;
        else
            _activeContext = _constructionContext;

        //phi::floatAnimator::update();
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
        label->setText(wstring(str.begin(), str.end()));

        if (a)
        {
            _constructionLabel->getComponent<phi::text>()->setText(L"edftg");
            _nandinhoLayer->add(_labelFps);
        }
        else
        {
            _labelFps->getParent()->removeChild(_labelFps);
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

        safeDelete(_designContext);
        safeDelete(_constructionContext);

        safeDelete(_nandinhoLayer);
        safeDelete(_constructionLayer);
        safeDelete(_sceneLayer);

        safeDelete(_sceneCamera);
        safeDelete(_nandinhoCamera);
        safeDelete(_constructionCamera);

#ifdef _DEBUG
        _watcher->endWatch();
        safeDelete(_watcher);
        safeDelete(_messageQueue);
#endif

        safeDelete(_framebufferAllocator);
        phi::framebuffer::release();
        phi::image::release();
        phi::material::release();
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