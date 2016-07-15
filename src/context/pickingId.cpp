#include <precompiled.h>
#include "pickingId.h"

namespace phi
{
    map<int, clickComponent*> pickingId::_ids = map<int, clickComponent*>();
    int pickingId::_currentId = 0;

    void pickingId::setNextId(clickComponent* click)
    {
        ++_currentId;

        click->setId(_currentId);
        _ids[_currentId] = click;
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