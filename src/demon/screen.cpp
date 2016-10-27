#include <precompiled.h>

#include <diagnostic/stopwatch.h>

#include <core/multiCommand.h>
#include <core/boxCollider.h>

#include <core/ghostMesh.h>

#include <input/input.h>

#include <loader/importer.h>
#include <loader/exporter.h>

#include <rendering/defaultFramebuffer.h>
#include <rendering/pickingFramebuffer.h>

#ifdef _DEBUG
#include <rendering/liveShaderReloader.h>
#endif

#include <ui/labelBuilder.h>
#include <ui/buttonBuilder.h>
#include <ui/switchControlBuilder.h>
#include <ui/control.h>
#include <ui/text.h>
#include <ui/relativeLayoutPosition.h>

#include <application/application.h>
#include <application/undoCommand.h>
#include <application/redoCommand.h>

#include <layers/layerBuilder.h>
#include <layers/nodeCreation\deleteNodeCommand.h>

#include <context/invalidLayerConfigurationException.h>

#include <ui/switchControl.h>

#include "screen.h"
#include "changeContextCommand.h"
#include "changeNodeTransformModeCommand.h"

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

        _labelFps = labelBuilder::newLabel(L"Fps: 0")
            .withPosition(vec3(-280.f, 100.f, 0.f))
            .withControlColor(1.0f, 0.0f, 1.0f, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(fontFps)
            .build();

        _labelNandinho = labelBuilder::newLabel(L"nanddiinho")
            .withPosition(vec3(-280.f, 80.f, 0.f))
            .withControlColor(1.0f, 0.0f, 0.0f, 1.0f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        auto changeContextButton = buttonBuilder::newButton()
            .withPosition(vec3(-280.f, 60.f, 0.f))
            .withText(L"Change context")
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .withControlColor(.5, .5, .2f, 1.f)
            .withAction([=](node* node)
        {
            //_commandsManager->executeCommand(new changeContextCommand());
        })
            .build();

        _constructionLabel = labelBuilder::newLabel(L"construction")
            .withPosition(vec3(-280.f, 100.f, 0.f))
            .withControlColor(0.0f, 1.0f, 0.5, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();

        _chair0 = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        _chair0->getTransform()->setLocalPosition(vec3(4.f, 3.0f, 0.0f));
        //_chair0->getTransform()->roll(-PI_OVER_2);

        auto cube0 = _userLibrary->getObjectsRepository()->getAllResources()[7]->getClonedObject();
        //cube0->getTransform()->yaw(PI_OVER_4);
        cube0->getTransform()->setLocalPosition(vec3(0.45f, 0.5f, 0.0f));
        //cube0->getTransform()->setLocalSize(vec3(0.5f, 0.5f, 0.5f));

        auto cube1 = _userLibrary->getObjectsRepository()->getAllResources()[7]->getClonedObject();
        cube1->getTransform()->setLocalPosition(vec3(-2.0f, 1.0f, 0.0f));

        auto back_wall = _userLibrary->getObjectsRepository()->getAllResources()[21]->getClonedObject();
        back_wall->getTransform()->setLocalPosition(vec3(0.0f, DECIMAL_TRUNCATION, -2.4f));
        auto floor0 = _userLibrary->getObjectsRepository()->getAllResources()[24]->getClonedObject();
        //floor0->getTransform()->setLocalPosition(vec3(0.0f, -5.0f, 0.0f));

        auto coffeTable = _userLibrary->getObjectsRepository()->getAllResources()[29]->getClonedObject();
        coffeTable->getTransform()->translate(vec3(5.0f, 0.0f, 0.0f));
        auto tableChair = _userLibrary->getObjectsRepository()->getAllResources()[5]->getClonedObject();
        tableChair->getTransform()->translate(vec3(-5.0f, 0.0f, 0.0f));
        auto table = _userLibrary->getObjectsRepository()->getAllResources()[28]->getClonedObject();
        table->getTransform()->translate(vec3(10.0f, 0.0f, 0.0f));

        _sceneCamera = new camera(_resolution, 0.1f, 1000.0f, PI_OVER_4);
        _sceneCamera->getTransform()->setLocalPosition(vec3(0.0f, 2.0f, 4.0f));
        _sceneCamera->getTransform()->yaw(PI);

        _translationImage = importer::importImage(application::resourcesPath + "/images/translation.png");
        _rotationImage = importer::importImage(application::resourcesPath + "/images/rotation.png");

        try
        {
            auto sceneLayerBuilder = layerBuilder::newLayer(_sceneCamera, application::resourcesPath, _framebufferAllocator, _commandsManager)
                .withMeshRenderer()
                .withGhostMeshRenderer()
                .withTranslationPlaneGridRenderer()
                .withRotationPlaneGridRenderer()
                .withPhysics()
                .withAnimation()
                .withCameraController()
                .withSelectionController()
                .withRotationController()
                .withTranslationController()
                .withBoxColliderRenderer();

            _sceneLayer = sceneLayerBuilder.build();
            _sceneLayer->addOnNodeSelectionChanged(std::bind(&screen::onNodeSelectionChanged, this, std::placeholders::_1));

            _translationController = sceneLayerBuilder.translationInputController;
            _translationController->translationStarted += std::bind(&screen::hideOnDemandUi, this);
            _translationController->translationEnded += std::bind(&screen::showOnDemandUi, this);
            _rotationController = sceneLayerBuilder.rotationInputController;
            _rotationController->rotationStarted += std::bind(&screen::hideOnDemandUi, this);
            _rotationController->rotationEnded += std::bind(&screen::showOnDemandUi, this);
            _rotationController->disable();
            _selectionBehaviour = sceneLayerBuilder.selectionLayerBehaviour;

            _constructionCamera = new camera(_resolution, 0.1f, 1000.0f, PI_OVER_4);
            _constructionCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
            _constructionCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

            _constructionLayer = layerBuilder::newLayer(_constructionCamera, application::resourcesPath, _framebufferAllocator, _commandsManager)
                .withControlRenderer()
                .withTextRenderer()
                .build();

            _nandinhoCamera = new camera(_resolution, 0.1f, 1000.0f, PI_OVER_4);
            _nandinhoCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 600.0f));

            _nandinhoLayer = layerBuilder::newLayer(_nandinhoCamera, application::resourcesPath, _framebufferAllocator, _commandsManager)
                .withControlRenderer()
                .withTextRenderer()
                .withUIMouseController()
                .build();
        }
        catch (const phi::invalidLayerConfigurationException& ex)
        {
            phi::application::logError(ex.what());
        }

        auto saveProjectButton = buttonBuilder::newButton()
            .withPosition(vec3(-280.f, 20.f, 0.f))
            .withText(L"Save project")
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .withControlColor(.3f, .9f, .2f, 1.f)
            .withAction([=](node* node)
        {
            OPENFILENAME ofn;

            char szFileName[MAX_PATH] = "";

            ZeroMemory(&ofn, sizeof(ofn));

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFilter = (LPCSTR)"Phi Files (*.phi)\0";
            ofn.lpstrFile = (LPSTR)szFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
            ofn.lpstrDefExt = (LPCSTR)"phi";

            GetSaveFileName(&ofn);
            auto path = ofn.lpstrFile;

            exporter::exportScene(_sceneLayer->getRoot(), path);
        })
            .build();

        auto loadProjectButton = buttonBuilder::newButton()
            .withPosition(vec3(-280.f, 40.f, 0.f))
            .withText(L"Load project")
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .withControlColor(.7f, .1f, .2f, 1.f)
            .withAction([=](node* node)
        {
            OPENFILENAME ofn;
            char fileNameBuffer[260];

            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.lpstrFile = fileNameBuffer;

            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(fileNameBuffer);
            ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            // Display the Open dialog box.

            if (GetOpenFileName(&ofn) == TRUE)
            {
                auto nodes = importer::loadPhiFile(ofn.lpstrFile, _userLibrary->getObjectsRepository());
                for (auto& node : nodes)
                {
                    _sceneLayer->add(node);
                }
            }
        }).build();

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
        _sceneLayer->add(_chair0);
        _sceneLayer->add(floor0);
        //_sceneLayer->add(back_wall);
        //_sceneLayer->add(table);
        //_sceneLayer->add(tableChair);
        //_sceneLayer->add(coffeTable);

        //TODO: prevent components that are not dealt with it from being added to layer

        //_constructionLayer->add(_constructionLabel);

        _onDemandUi = createOnDemandUiNode();
        _onDemandUi->addComponent(new relativeLayoutPosition(_nandinhoCamera, _sceneLayer));

        _nandinhoLayer->add(_labelNandinho);
        _nandinhoLayer->add(_labelFps);
        _nandinhoLayer->add(changeContextButton);
        _nandinhoLayer->add(loadProjectButton);
        _nandinhoLayer->add(saveProjectButton);

        _activeContext = _designContext;
    }

    void screen::initInput()
    {
        input::mouseDown->assign(std::bind(&screen::onMouseDown, this, std::placeholders::_1));
        input::mouseUp->assign(std::bind(&screen::onMouseUp, this, std::placeholders::_1));
        input::mouseClick->assign(std::bind(&screen::onMouseClick, this, std::placeholders::_1));
        input::mouseDoubleClick->assign(std::bind(&screen::onMouseDoubleClick, this, std::placeholders::_1));
        input::mouseMove->assign(std::bind(&screen::onMouseMove, this, std::placeholders::_1));
        input::beginMouseWheel->assign(std::bind(&screen::onBeginMouseWheel, this, std::placeholders::_1));
        input::mouseWheel->assign(std::bind(&screen::onMouseWheel, this, std::placeholders::_1));
        input::endMouseWheel->assign(std::bind(&screen::onEndMouseWheel, this, std::placeholders::_1));
        input::keyDown->assign(std::bind(&screen::onKeyDown, this, std::placeholders::_1));
        input::keyUp->assign(std::bind(&screen::onKeyUp, this, std::placeholders::_1));

        _commandsManager = new commandsManager();
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_r }, [&]() { return new changeNodeTransformModeCommand(); }));
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_z }, [&]() { return new undoCommand(_commandsManager); }));
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_y }, [&]() { return new redoCommand(_commandsManager); }));
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_SPACE }, [&]() { return new changeContextCommand(); }));
    }

    void screen::onMouseDown(phi::mouseEventArgs* e)
    {
        _activeContext->onMouseDown(e);
    }

    void screen::onMouseUp(phi::mouseEventArgs* e)
    {
        _activeContext->onMouseUp(e);
    }

    void screen::onMouseClick(phi::mouseEventArgs * e)
    {
        _activeContext->onMouseClick(e);
    }

    void screen::onMouseDoubleClick(phi::mouseEventArgs* e)
    {
        _activeContext->onMouseDoubleClick(e);
    }

    void screen::onMouseMove(phi::mouseEventArgs* e)
    {
        _activeContext->onMouseMove(e);
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

    void screen::onUpdate()
    {
        if (_design)
            _activeContext = _designContext;
        else
            _activeContext = _constructionContext;

        _activeContext->update();
    }

    void screen::onRender()
    {
        _activeContext->render();
    }

    void screen::onTick()
    {
        auto label = _labelFps->getComponent<phi::text>();
        auto str = "Fps: " + std::to_string(application::framesPerSecond);
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

    node* screen::createOnDemandUiNode()
    {
        auto switchControl = switchControlBuilder::newSwitchControl()
            .withSize(vec3(15.0f, 30.0f, 0.1f))
            .withOptionAImage(_translationImage)
            .withOptionBImage(_rotationImage)
            .withOptionACallback([&]()
        {
            _translationController->enable();
            _rotationController->disable();
        })
            .withOptionBCallback([&]()
        {
            _translationController->disable();
            _rotationController->enable();
        }).build();

        return switchControl;
    }

    void screen::showOnDemandUi()
    {
        _nandinhoLayer->add(_onDemandUi);
        _onDemandUi->getComponent<relativeLayoutPosition>()->updatePosition();
    }

    void screen::hideOnDemandUi()
    {
        _onDemandUi->getParent()->removeChild(_onDemandUi);
    }

    void screen::onNodeSelectionChanged(node* node)
    {
        auto selectedNodes = _selectionBehaviour->getSelectedNodes();
        auto selectedNodesCount = selectedNodes->size();

        auto relativePositionComponent = _onDemandUi->getComponent<relativeLayoutPosition>();
        auto previousTargetNode = relativePositionComponent->getTargetNode();

        if (selectedNodesCount > 0)
        {
            auto lastNode = (*selectedNodes)[selectedNodesCount - 1];
            relativePositionComponent->setTargetNode(lastNode);

            if (!previousTargetNode)
                showOnDemandUi();
        }
        else
        {
            relativePositionComponent->setTargetNode(nullptr);
            if (previousTargetNode)
                hideOnDemandUi();
        }
    }
}