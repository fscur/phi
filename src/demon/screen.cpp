#include <precompiled.h>

#include <diagnostic/stopwatch.h>

#include <core/multiCommand.h>
#include <core/boxCollider.h>

#include <core/ghostMesh.h>

#include <input/input.h>

#include <data/abstractions/iModelDataService.h>
#include <data/geometryRepository.h>
#include <data/materialRepository.h>
#include <data/textureRepository.h>
#include <data/modelRepository.h>
#include <data/modelDataService.h>

#include <io/path.h>

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
#include <layers/nodeCreation/deleteNodeCommand.h>

#include <context/invalidLayerConfigurationException.h>

#include <ui/switchControl.h>

#include <persistence/projectRepository.h>

#include "screen.h"
#include "changeContextCommand.h"
#include "changeNodeTransformModeCommand.h"
#include <core/skyBox.h>

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
        auto textureRepository = new phi::textureRepository(application::libraryPath);
        auto materialRepository = new phi::materialRepository(application::libraryPath, textureRepository);
        auto geometryRepository = new phi::geometryRepository(application::libraryPath);
        auto modelRepository = new phi::modelRepository(application::libraryPath);
        auto modelDataService = new phi::modelDataService(
            geometryRepository,
            materialRepository,
            textureRepository,
            modelRepository);

        _userLibrary = new library(modelDataService);
        _projectRepository = new projectRepository();
    }

    void screen::initContexts()
    {
        auto font = fontsManager::load("Roboto-Thin.ttf", 10);
        auto fontFps = fontsManager::load("Roboto-Thin.ttf", 12);

        _labelFps = labelBuilder::newLabel(L"Fps: 0")
            .withPosition(vec3(-280.f, 120.f, 0.f))
            .withGlassyLook()
            .withControlColor(1.0f, 0.0f, 1.0f, 0.3f)
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
            _unused(node);
            _commandsManager->executeCommand(new changeContextCommand());
        })
            .build();

        _constructionLabel = labelBuilder::newLabel(L"construction")
            .withPosition(vec3(-280.f, 100.f, 0.f))
            .withControlColor(0.0f, 1.0f, 0.5, 1.f)
            .withTextColor(1.f, 1.f, 1.f, 1.f)
            .withFont(font)
            .build();


        _sceneCamera = new camera(_resolution, 0.1f, 1000.0f, PI_OVER_4);
        _sceneCamera->getTransform()->setLocalPosition(vec3(0.0f, 2.0f, -5.0f));
        _sceneCamera->getTransform()->pitch(0.3f);

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
                .withSkyBoxRenderer();

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

            _scene = new phi::scene(_sceneLayer, _sceneCamera);
            _project = new project(_scene);

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
            _unused(node);
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

            _projectRepository->save(_project, path);
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
            _unused(node);
            //OPENFILENAME ofn;
            //char fileNameBuffer[260];

            //ZeroMemory(&ofn, sizeof(ofn));
            //ofn.lStructSize = sizeof(ofn);
            //ofn.lpstrFile = fileNameBuffer;

            //ofn.lpstrFile[0] = '\0';
            //ofn.nMaxFile = sizeof(fileNameBuffer);
            //ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
            //ofn.nFilterIndex = 1;
            //ofn.lpstrFileTitle = NULL;
            //ofn.nMaxFileTitle = 0;
            //ofn.lpstrInitialDir = NULL;
            //ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            // Display the Open dialog box.

            //if (GetOpenFileName(&ofn) == TRUE)
            //{
            //    auto nodes = importer::loadPhiFile(ofn.lpstrFile, _userLibrary->getObjectsRepository());
            //    for (auto& node : nodes)
            //    {
            //        _sceneLayer->add(node);
            //    }
            //}
        })
            .build();

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

        node* skyBoxNode = new node();
        skyBoxImages images;
        images.PositiveX = importer::importImage("D:\\Phi\\resources\\images\\skyboxes\\SunnyDay\\PositiveX.png");
        images.NegativeX = importer::importImage("D:\\Phi\\resources\\images\\skyboxes\\SunnyDay\\NegativeX.png");
        images.PositiveY = importer::importImage("D:\\Phi\\resources\\images\\skyboxes\\SunnyDay\\PositiveY.png");
        images.NegativeY = importer::importImage("D:\\Phi\\resources\\images\\skyboxes\\SunnyDay\\NegativeY.png");
        images.PositiveZ = importer::importImage("D:\\Phi\\resources\\images\\skyboxes\\SunnyDay\\PositiveZ.png");
        images.NegativeZ = importer::importImage("D:\\Phi\\resources\\images\\skyboxes\\SunnyDay\\NegativeZ.png");
        
        skyBox* skyBoxComp = new skyBox(images);
        skyBoxNode->addComponent(skyBoxComp);

        _sceneLayer->add(skyBoxNode);

        auto obj0 = _userLibrary->getModelByIndex(3); //chair
        obj0->getTransform()->yaw(phi::PI_OVER_2);
        //auto obj1 = _userLibrary->getModelByIndex(24); // floor
        
        _scene->add(obj0);
        //_scene->add(obj1);

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

        safeDelete(_designContext);
        safeDelete(_constructionContext);

        safeDelete(_projectRepository);
        safeDelete(_project);
        safeDelete(_nandinhoLayer);
        safeDelete(_constructionLayer);

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
        _unused(node);
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