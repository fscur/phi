#pragma once
#include <phi.h>
#include "contextApi.h"

#include <common\mouseInteractionComponent.h>

namespace phi
{
    class pickingId
    {
    private:
        static int _currentId;
        static map<int, mouseInteractionComponent*> _ids;
    public:
        CONTEXT_API static void setNextId(mouseInteractionComponent* mouseInteractionComponent);
        CONTEXT_API static mouseInteractionComponent* get(int id);
        CONTEXT_API static map<int, mouseInteractionComponent*>* getAll();
        CONTEXT_API static void remove(mouseInteractionComponent* click);
    };
}