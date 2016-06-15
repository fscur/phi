#include <precompiled.h>
#include "liveShaderReloader.h"

#include <io\path.h>

namespace phi
{
    map<string, vector<program*>> liveShaderReloader::_programs;

    void liveShaderReloader::reloadShader(string fileName)
    {
        auto it = _programs.find(fileName);
        if (it != _programs.end())
        {
            auto programsToReload = it->second;
            for (auto& program : programsToReload)
            {
                if (program->canCompile())
                {
                    debug("<liveShaderReloader> " + fileName + " reloaded.");
                    program->reload();
                }
                else
                {
                    debug("<liveShaderReloader> failed to reload " + fileName);
                }
            }
        }
    }

    void liveShaderReloader::add(string shaderFileName, program * program)
    {
        _programs[shaderFileName].push_back(program);
    }
}