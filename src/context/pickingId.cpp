#include <precompiled.h>
#include "pickingId.h"

namespace phi
{
    map<int, clickComponent*> pickingId::_ids = map<int, clickComponent*>();
    int pickingId::_currentId = 1;

    void pickingId::setNextId(clickComponent* click)
    {
        click->setId(_currentId);
        _ids[_currentId] = click;
        ++_currentId;
    }

    clickComponent* pickingId::get(int id)
    {
        return _ids[id];
    }

    void pickingId::remove(clickComponent* click)
    {
        _ids.erase(click->getId());
    }
}