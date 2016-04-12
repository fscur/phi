#include <precompiled.h>
#include "shadersManager.h"

#include <io\path.h>

namespace phi
{
    shadersManager::shadersManager(string path) :
        _path(path)
    {}

    shadersManager::~shadersManager()
    {
        for (auto& pair : _shaders)
        {
            pair.second->release();
            safeDelete(pair.second);
        }
    }

    shader* shadersManager::load(string name, const vector<string>& attributes)
    {
        auto vertFile = path::combine(_path, name, ".vert");
        auto fragFile = path::combine(_path, name, ".frag");

        auto shader = new phi::shader(vertFile, fragFile, attributes);
        shader->init();
        _shaders[name] = shader;

        return shader;
    }

    shader* shadersManager::get(string name)
    {
        if (_shaders.find(name) == _shaders.end())
            return nullptr;
        else
            return _shaders[name];
    }
}