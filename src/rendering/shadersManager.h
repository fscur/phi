#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "program.h"

namespace phi
{
    class shadersManager
    {
    private:
        string _path;
        std::map<string, program*> _programs;

    public:
        shadersManager(string path);
        ~shadersManager();
        static constexpr const char* VERT_EXT = ".vert";
        static constexpr const char* FRAG_EXT = ".frag";

        //RENDERING_API program* loadCrazyFuckerSpecificShader(const string& name);

        //RENDERING_API program* load(const string& name);
        RENDERING_API void reloadAllPrograms();
		RENDERING_API void reloadProgram(string name);
    };
}