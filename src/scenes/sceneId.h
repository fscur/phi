#pragma once
#include <phi.h>

namespace phi
{
    class sceneId
    {
    private:
        static int _currentId;
    public:
        static int next()
        {
            ++_currentId;
            return _currentId;
        }
    };

    int sceneId::_currentId = 0;
}