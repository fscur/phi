#include <precompiled.h>
#include "command.h"

namespace phi
{
    command::command()
    {
        _finishedCallback = nullptr;
    }

    command::~command()
    {
    }

    void command::start()
    {
        execute();
    }

    void command::startUndo()
    {
        executeUndo();
    }

    void command::startAsync(std::function<void(command*)> finishedCallback)
    {
        if (_thread.joinable())
            _thread.join();
        _finishedCallback = finishedCallback;
        _thread = std::thread(&command::threadStart, this);
    }

    void command::startUndoAsync(std::function<void(command*)> finishedCallback)
    {
        if (_thread.joinable())
            _thread.join();
        _finishedCallback = finishedCallback;
        _thread = std::thread(&command::threadStartUndo, this);
    }

    void command::threadStart()
    {
        execute();
        if (_finishedCallback != nullptr)
            _finishedCallback(this);
    }

    void command::threadStartUndo()
    {
        executeUndo();
        if (_finishedCallback != nullptr)
            _finishedCallback(this);
    }
}