#ifndef _PHI_COMMAND_H_
#define _PHI_COMMAND_H_

#include <glm\glm.hpp>
#include "size.h"
#include <functional>
#include <thread>

class commandInfo
{
public:
    glm::vec2 mouseDownPos;
    glm::vec2 mousePos;
    glm::vec2 lastMousePos;
    float wheelDelta;
    phi::size<unsigned int> viewportSize;
    unsigned long actionsSum;
public:
    virtual ~commandInfo(){}
};

class command
{
private:
    std::thread _thread;
    std::function<void(command*)> _finishedCallback;

private:
    void threadStart();
    void threadStartUndo();

protected:
    virtual void execute() { };
    virtual void executeUndo() { };

public:
    command();
    virtual bool getIsUndoable() { return true; }
    void start();
    void startUndo();
    void startAsync(std::function<void(command*)> finishedCallback);
    void startUndoAsync(std::function<void(command*)> finishedCallback);
};

#endif