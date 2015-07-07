#ifndef _PHI_COMMAND_H_
#define _PHI_COMMAND_H_

#include <glm\glm.hpp>
#include "size.h"

namespace phi
{
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
    public:
        commandInfo* info;

    public:
        command(){}
        virtual void init() {}
        virtual bool canExecute() { return true; }
        virtual void update() { };
        virtual void finish() { };
    };
}
#endif