#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "shader.h"

namespace phi
{
    class shadersManager
    {
    private:
        string _path;
        std::map<string, shader*> _shaders;

    public:
        shadersManager(string path);
        ~shadersManager();

        shader* load(string name, const vector<string>& attributes);
        shader* get(string name);
    };
}