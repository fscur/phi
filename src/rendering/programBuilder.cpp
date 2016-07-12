#include <precompiled.h>
#include "programBuilder.h"

#include <io\path.h>
#include <io\directoryInfo.h>

#include <rendering\liveShaderReloader.h>
#include <rendering\glslCompiler.h>

namespace phi
{
    program* programBuilder::buildProgram(
        const string& shadersPath,
        const string& vertexShaderName,
        const string& fragmentShaderName)
    {
        auto vertexFileName = path::combine(shadersPath, vertexShaderName, shader::VERT_EXT);
        auto fragmentFileName = path::combine(shadersPath, fragmentShaderName, shader::FRAG_EXT);

        //TODO: check if file exists

        auto vertexShader = new shader(vertexFileName);
        auto fragmentShader = new shader(fragmentFileName);

        auto program = new phi::program();
        program->addShader(vertexShader);
        program->addShader(fragmentShader);
        program->compile();
        program->link();

#ifdef _DEBUG
        liveShaderReloader::add(vertexFileName, program);
        liveShaderReloader::add(fragmentFileName, program);
#endif
        return program;
    }
}