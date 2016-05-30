#include <precompiled.h>
#include "shadersManager.h"

#include <io\path.h>

namespace phi
{
    shadersManager::shadersManager(string path) :
        _path(path)
    {
    }

    shadersManager::~shadersManager()
    {
        for (auto& pair : _programs)
            safeDelete(pair.second);
    }

    program* shadersManager::load(string name, const vector<string>& attributes)
    {
        auto vertFile = path::combine(_path, name, shadersManager::VERT_EXT);
        auto fragFile = path::combine(_path, name, shadersManager::FRAG_EXT);

        auto vertexShader = new phi::shader(vertFile);
        auto fragmentShader = new phi::shader(fragFile);

        auto program = new phi::program();

        program->add(vertexShader);
        program->add(fragmentShader);

        _programs[name] = program;

        return program;
    }

    program* shadersManager::get(string name)
    {
        if (_programs.find(name) == _programs.end())
            return nullptr;
        else
            return _programs[name];
    }

    void shadersManager::reloadAllPrograms()
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

    void shadersManager::reloadProgram(string programName)
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