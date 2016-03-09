#pragma once
#include <phi.h>
#include <core\size.h>

namespace demon
{
    class commandInfo
    {
    public:
        glm::vec2 mouseDownPos;
        glm::vec2 mousePos;
        glm::vec2 lastMousePos;
        float wheelDelta;
        phi::sizeui viewportSize;
        unsigned long actionsSum;
    public:
        virtual ~commandInfo() {}
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
}