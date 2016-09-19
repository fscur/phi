#include <precompiled.h>
#include "pickingId.h"

namespace phi
{
    map<int, mouseInteractionComponent*> pickingId::_ids = map<int, mouseInteractionComponent*>();
    int pickingId::_currentId = 1;

    void pickingId::setNextId(mouseInteractionComponent* mouseInteractionComponent)
    {
        mouseInteractionComponent->setId(_currentId);
        _ids[_currentId] = mouseInteractionComponent;
        ++_currentId;
    }

    mouseInteractionComponent* pickingId::get(int id)
    {
        return _ids[id];
    }

    map<int, mouseInteractionComponent*>* pickingId::getAll()
    {
        return &_ids;
    }

    void pickingId::remove(mouseInteractionComponent* click)
    {
        _ids.erase(click->getId());
    }
}