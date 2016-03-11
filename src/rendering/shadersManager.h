#pragma once
#include <phi.h>
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
        shadersManager(string path) :
            _path(path) 
        {}

        ~shadersManager();
        shader* load(string name, vector<string> attributes);
        shader* get(string name);
    };
}