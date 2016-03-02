#include <precompiled.h>
#include "shadersManager.h"

#include <io\path.h>

namespace phi
{
    shadersManager::~shadersManager()
    {
        for (auto pair : _shaders)
        {
            pair.second->release();
            safeDelete(pair.second);
        }
    }

    shader* shadersManager::load(string name, vector<string> attributes)
    {
        auto vertFile = path::combine(_path, name, ".vert");
        auto fragFile = path::combine(_path, name, ".frag");

        shader* s = new shader(vertFile, fragFile, attributes);
        s->init();
        _shaders[name] = (s);

        return s;
    }

    shader* shadersManager::get(string name)
    {
        if (_shaders.find(name) == _shaders.end())
            return nullptr;
        else
            return _shaders[name];
    }
}