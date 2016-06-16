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
        auto vertexFileName = path::combine(shadersPath, vertexShaderName, shader::VERT_EXT);
        auto fragmentFileName = path::combine(shadersPath, fragmentShaderName, shader::FRAG_EXT);

        //TODO: check if file exists

        auto newShader = [&](string fileName)
        {
            if (contains(_shadersCache, fileName))
                return _shadersCache[fileName];

            auto shader = new phi::shader(fileName);
            _shadersCache[fileName] = shader;

            return shader;
        };

        auto vertexShader = newShader(vertexFileName);
        auto fragmentShader = newShader(fragmentFileName);

        auto program = new phi::program();
        program->addShader(vertexShader);
        program->addShader(fragmentShader);
        program->link();

#ifdef _DEBUG
        liveShaderReloader::add(vertexFileName, program);
        liveShaderReloader::add(fragmentFileName, program);
#endif

        return program;
    }
}