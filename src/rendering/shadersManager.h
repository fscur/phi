#pragma once
#include <precompiled.h>
#include "rendering.h"
#include "shader.h"

namespace phi
{
    class shadersManager
    {
    private:
        string _path;
        std::map<string, shader*> _shaders;

    public:
        RENDERING_API shadersManager(string path) :
            _path(path) 
        {}

        RENDERING_API ~shadersManager();
        RENDERING_API shader* load(string name, vector<string> attributes);
        RENDERING_API shader* get(string name);
    };
}