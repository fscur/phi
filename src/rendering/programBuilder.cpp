#include <precompiled.h>
#include "programBuilder.h"

#include <io\path.h>

#include <rendering\liveShaderReloader.h>
#include <rendering\glslCompiler.h>

namespace phi
{
    map<string, shader*> programBuilder::_shadersCache = map<string, shader*>();
    map<std::tuple<shader*, shader*>, program*> programBuilder::_programsCache = map<std::tuple<shader*, shader*>, program*>();

    program* programBuilder::buildProgram(
        const string& shadersPath,
        const string& vertexShaderName,
        const string& fragmentShaderName)
    {
        auto vertexShaderFileName = path::combine(shadersPath, vertexShaderName, shader::VERT_EXT);
        auto fragmentShaderFileName = path::combine(shadersPath, fragmentShaderName, shader::FRAG_EXT);

        auto newShader = [&](string fileName)
        {
            phi::shader* shader = nullptr;

            if (_shadersCache.find(fileName) != _shadersCache.end())
            {
                shader = _shadersCache[fileName];
            }
            else
            {
                shader = new phi::shader(fileName);
                _shadersCache[fileName] = shader;
#ifdef _DEBUG
                liveShaderReloader::shaders[fileName] = shader;
#endif
            }

            return shader;
        };

        auto vertexShader = newShader(vertexShaderFileName);
        auto fragmentShader = newShader(fragmentShaderFileName);

        phi::program* program = nullptr;
        auto key = std::tuple<shader*, shader*>(vertexShader, fragmentShader);

        if (_programsCache.find(key) != _programsCache.end())
            program = _programsCache[key];
        else
        {
            program = glslCompiler::compile({ vertexShader, fragmentShader });
            _programsCache[key] = program;
        }

        return program;
    }
}