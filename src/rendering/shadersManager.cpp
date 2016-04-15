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
            safeDelete(pair.second);
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

    void shadersManager::reloadAllShaders()
    {
        for (auto& pair : _shaders)
        {
            auto shader = pair.second;
            while (!shader->reload())
            {
                phi::debug("\nFailed \""+ pair.first + "\" shader compilation.\nPress enter to try again...\n");
                std::cin.get();
            }
        }
    }
}