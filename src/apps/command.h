#pragma once
#include <phi.h>
#include "appsApi.h"

#include <core\size.h>

namespace phi
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
        APPS_API virtual ~commandInfo();
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
        APPS_API command();
        virtual bool getIsUndoable() { return true; }
        APPS_API void start();
        APPS_API void startUndo();
        APPS_API void startAsync(std::function<void(command*)> finishedCallback);
        APPS_API void startUndoAsync(std::function<void(command*)> finishedCallback);
    };
}