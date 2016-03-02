#include <precompiled.h>
#include "commandsManager.h"
//#include "selectAtPositionCommand.h"
//#include "undoCommand.h"
//#include "redoCommand.h"
//#include "zoomToFitCommand.h"
//#include "deleteSceneObjectCommand.h"




commandsManager::commandsManager()
{
    /*_shortcuts.add("Select", phi::inputKey(PHI_MOUSE_LEFT, PHI_NONE));
    _shortcuts.add("DeselectAll", phi::inputKey(PHI_SCANCODE_ESCAPE, PHI_NONE));
    _shortcuts.add("Undo", phi::inputKey(PHI_SCANCODE_Z, PHI_CTRL_PRESSED));
    _shortcuts.add("Redo", phi::inputKey(PHI_SCANCODE_Y, PHI_CTRL_PRESSED));
    _shortcuts.add("ZoomToFit", phi::inputKey(PHI_SCANCODE_F, PHI_CTRL_PRESSED));
    _shortcuts.add("DeleteSceneObject", phi::inputKey(PHI_SCANCODE_DELETE, PHI_NONE));*/

    //_commands["Select"] = [] () -> command*
    //{
    //    int mouseX, mouseY;
    //    SDL_GetMouseState(&mouseX, &mouseY);
    //    return new selectAtPositionCommand(glm::vec2(mouseX, mouseY));
    //};

    //_commands["DeselectAll"] = [&]() -> command* { return new selectObjectCommand(nullptr, 0); };
    //_commands["Undo"] = [&] () -> command* { return new undoCommand(this); };
    //_commands["Redo"] = [&] () -> command* { return new redoCommand(this); };
    //_commands["ZoomToFit"] = [&] () -> command* { return new zoomToFitCommand(); };
    //_commands["DeleteSceneObject"] = [&]() -> command* { return new deleteSceneObjectCommand(); };

    _executingCommand = false;
}

commandsManager::~commandsManager()
{
}

void commandsManager::enqueueCommand(command* cmd, std::function<void(command*)> callback, bool isUndo)
{
    _pendingCommandsMutex.lock();
    _pendingCommands.push(new pendingCommand(cmd, callback, isUndo));
    _pendingCommandsMutex.unlock();
}

void commandsManager::startNextCommand()
{
    _pendingCommandsMutex.lock();
    if (!_executingCommand && !_pendingCommands.empty())
    {
        _executingCommand = true;
        pendingCommand* pendingCmd = _pendingCommands.front();
        if (!pendingCmd->isUndo)
            pendingCmd->cmd->startAsync(pendingCmd->callback);
        else
            pendingCmd->cmd->startUndoAsync(pendingCmd->callback);
        _pendingCommands.pop();
    }
    _pendingCommandsMutex.unlock();
}

void commandsManager::undo()
{
    if (_undo.empty())
        return;

    auto cmd = _undo.top();
    _undo.pop();

    auto callback = [&] (command* c)
    {
        _redo.push(c);
        _executingCommand = false;
        startNextCommand();
    };
    enqueueCommand(cmd, callback, true);
}

void commandsManager::redo()
{
    if (_redo.empty())
        return;

    auto cmd = _redo.top();
    _redo.pop();

    auto callback = [&] (command* c)
    {
        _undo.push(c);
        _executingCommand = false;
        startNextCommand();
    };
    enqueueCommand(cmd, callback, false);
}

void commandsManager::executeCommand(command* cmd)
{
    auto callback = [&] (command* c)
    {
        if (c->getIsUndoable())
        {
            _undo.push(c);
            /*std::stack<command*> empty;
            std::swap(_redo, empty);*/

            while (!_redo.empty())
            {
                _redo.pop();
            }
        }

        _executingCommand = false;
        startNextCommand();
    };
    enqueueCommand(cmd, callback, false);
    startNextCommand();
}

void commandsManager::update()
{
    //startNextCommand();
}

bool commandsManager::onInput(phi::inputKey key)
{
    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
    auto cmdName = _shortcuts[key];
    if (cmdName == "")
        return false;

    auto cmd = _commands[cmdName]();
    if (cmd != nullptr)
    {
        executeCommand(cmd);
        return true;
    }

    return false;
}