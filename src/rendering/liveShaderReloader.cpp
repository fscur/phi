#include <precompiled.h>
#include "liveShaderReloader.h"

#include <io\path.h>

namespace phi
{
    void liveShaderReloader::reloadAllPrograms()
    {
        for (auto& pair : _programs)
        {
            auto program = pair.second;
            while (!program->reload())
            {
                phi::debug("\n[reloadAllShaders] Failed \"" + pair.first + "\" program compilation.\nPress enter to try again...\n");
                std::cin.get();
            }
        }
    }

    void liveShaderReloader::reloadProgram(string programName)
    {
        auto selectedProgram = _programs[programName];
        if (selectedProgram)
        {
            if (selectedProgram->canCompile())
            {
                selectedProgram->reload();
                debug("<shadersManager> " + programName + " reloaded.");
            }
            else
            {
                debug("<shaderManager> program validation error.");
            }
        }
        else
        {
            debug("<shadersManager>: program not found.");
        }
    }
}