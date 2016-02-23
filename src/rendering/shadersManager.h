#pragma once

#include <core/globals.h>

#include "rendering.h"
#include "shader.h"

#include <map>
#include <string>

namespace phi
{
    class shadersManager
    {
    private:
        std::string _path;
        std::map<std::string, shader*> _shaders;

    public:
        RENDERING_API shadersManager(std::string path) :
            _path(path) 
        {}

        RENDERING_API ~shadersManager();
        RENDERING_API shader* load(std::string name, std::vector<std::string> attributes);
        RENDERING_API shader* get(std::string name);
    };
}