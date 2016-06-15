#include <precompiled.h>
#include "programBuilder.h"

#include <io\path.h>
#include <io\directoryInfo.h>

#include <rendering\liveShaderReloader.h>
#include <rendering\glslCompiler.h>

namespace phi
{
    map<string, shader*> programBuilder::_shadersCache = map<string, shader*>();

    program* programBuilder::buildProgram(
        const string& shadersPath,
        const string& vertexShaderName,
        const string& fragmentShaderName)
    {
        auto vertexShaderFileName = path::combine(shadersPath, vertexShaderName, shader::VERT_EXT);
        auto fragmentShaderFileName = path::combine(shadersPath, fragmentShaderName, shader::FRAG_EXT);

        //TODO: check if file exists

        auto newShader = [&](string fileName)
        {
            shader* shader = nullptr;

            if (_shadersCache.find(fileName) != _shadersCache.end())
            {
                shader = _shadersCache[fileName];
            }
            else
            {
                shader = new phi::shader(fileName);
                _shadersCache[fileName] = shader;
            }

            return shader;
        };

        auto vertexShader = newShader(vertexShaderFileName);
        auto fragmentShader = newShader(fragmentShaderFileName);

        auto program = new phi::program();
        program->addShader(vertexShader);
        program->addShader(fragmentShader);
        program->link();

#ifdef _DEBUG
        liveShaderReloader::add(vertexShaderFileName, program);
        liveShaderReloader::add(fragmentShaderFileName, program);
#endif

        return program;
    }
}