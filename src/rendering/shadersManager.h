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
        static constexpr const char* VERT_EXT = ".vert";
        static constexpr const char* FRAG_EXT = ".frag";

        RENDERING_API shader* loadCrazyFuckerSpecificShader(const string& name);
        RENDERING_API shader* load(string name, const vector<string>& attributes);
        RENDERING_API shader* get(string name);
        RENDERING_API void reloadAllShaders();
        RENDERING_API void reloadShader(string vertFileName);
    };
}