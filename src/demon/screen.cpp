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
#include <context\contextBuilder.h>

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
    }

    void screen::initUi()
    {
        auto font = _gl->fontsManager->load("Roboto-Thin.ttf", 14);
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

        auto floor = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        auto chair = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair->getTransform()->setLocalPosition(vec3(0.f, .1f, .0f));

        auto sceneLayer = layerBuilder::buildScene(application::resourcesPath, _gl, (float) _width, (float)_height);
        sceneLayer->add(floor);
        sceneLayer->add(chair);

        auto nandinhoLayer = layerBuilder::buildUI(application::resourcesPath, _gl, (float) _width, (float)_height);
        nandinhoLayer->add(labelNandinho);
        nandinhoLayer->add(_labelFps);

        auto constructionLayer = layerBuilder::buildUI(application::resourcesPath, _gl, (float)_width, float(_height));
        constructionLayer->add(constructionLabel);

        _designContext = new context({ sceneLayer, nandinhoLayer });
        _constructionContext = new context({ sceneLayer, constructionLayer });

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

    void screen::onUpdate()
    {
        phi::floatAnimator::update();

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