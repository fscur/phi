#pragma once
#include <phi.h>
#include "layersApi.h"

#include <common\mouseInteractionComponent.h>

namespace phi
{
    class pickingId
    {
    private:
        static int _currentId;
        static map<int, mouseInteractionComponent*> _ids;
    public:
        LAYERS_API static void setNextId(mouseInteractionComponent* mouseInteractionComponent);
        LAYERS_API static mouseInteractionComponent* get(int id);
        LAYERS_API static map<int, mouseInteractionComponent*>* getAll();
        LAYERS_API static void remove(mouseInteractionComponent* click);
    };
}