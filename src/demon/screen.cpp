#include <precompiled.h>
#include "screen.h"

#include <apps\undoCommand.h>
#include <apps\redoCommand.h>

#include <diagnostics\diagnostics.h>
#include <diagnostics\stopwatch.h>

#include <loader\importer.h>
#include <rendering\model.h>

namespace demon
{
    screen::screen(phi::string name, phi::uint width, phi::uint height) :
        window(name, width, height)
    {
        _temp = 0.0f;
        _isMouseDown = false;
        _rotating = false;
        _shadowMap = false;
        _translationSpeed = 1.0f;
        _rotationSpeed = 0.01f;
    }

    screen::~screen()
    {
    }

    void screen::onInitialize()
    {
        //initGL();
        //initLibrary();
        initScene();
        initInput();
    }

    void screen::initGL()
    {
        auto initState = phi::gl::state();
        initState.clearColor = phi::vec4(0.0f);
        initState.frontFace = phi::gl::frontFace::ccw;
        initState.culling = true;
        initState.cullFace = phi::gl::cullFace::back;
        initState.depthMask = true;
        initState.depthTest = true;
        initState.useBindlessTextures = false;
        initState.useSparseTextures = false;

        auto info = phi::gl::glInfo();
        info.state = initState;
        info.shadersPath = _resourcesPath + "/shaders";
        _gl = new phi::gl(info);
    }

    void screen::initLibrary()
    {
        _library = new library(_gl, _libraryPath);
        _library->init();
    }

    void screen::initScene()
    {
        _scene = new phi::scene(_gl, _width, _height);
        auto camera = _scene->camera;

        auto cameraTransform = camera->getTransform();
        auto cameraPos = phi::vec3(0.0f, 0.0f, 2.0f);
        cameraTransform->setLocalPosition(cameraPos);
        cameraTransform->setDirection(-cameraPos);

        //auto floor = _library->getObjectsRepository()->getAllResources()[24]->getObject();
        //auto clonedFloor = floor->clone();
        //_scene->add(clonedFloor);

        //auto obj = _library->getObjectsRepository()->getAllResources()[2]->getObject();
        //for (size_t i = 0; i < 10; i++)
        //{
        //    auto cloned = obj->clone();
        //    cloned->getTransform().setLocalPosition(phi::vec3(i + (0.1f*i), 0.0, 0.0));
        //    _scene->add(cloned);
        //}
    }

    class command1 : public phi::command
    {
    private:
        std::string _message;

    public:
        command1(std::string message) :
            _message(message)
        {
        }

        void execute() override
        {
            printf("[redo] %s\n", _message.c_str());
        }

        void executeUndo() override
        {
            printf("[undo] %s\n", _message.c_str());
        }
    };

    void screen::initInput()
    {
        _commandsManager = new phi::commandsManager();
        _commandsManager->addShortcut(phi::shortcut({ PHIK_LCTRL, PHIK_z }, [&]() -> phi::command* { return new phi::undoCommand(_commandsManager); }));
        _commandsManager->addShortcut(phi::shortcut({ PHIK_LCTRL, PHIK_y }, [&]() -> phi::command* { return new phi::redoCommand(_commandsManager); }));
        _commandsManager->addShortcut(phi::shortcut({ PHIK_a }, [&]() -> phi::command* { return new command1("forfeit"); }));
        _commandsManager->addShortcut(phi::shortcut({ PHIK_g }, [&]() -> phi::command* { return new command1("gg"); }));

        _defaultController = new defaultCameraController(_scene);
    }

    void screen::onUpdate()
    {
        //_scene->update();
    }

    void screen::onRender()
    {
        _scene->render();
    }

    void screen::onClosing()
    {
        //TODO: MessageBox asking if the user really wants to close the window
        //TODO: Check if we really need the above TODO
    }
}