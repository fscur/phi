#include <precompiled.h>
#include "shadersManager.h"

#include <io\path.h>
#include "glslCompiler.h"

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

    //program* shadersManager::loadCrazyFuckerSpecificShader(const string& name)
    //{
    //    return load(name);

    //    //if (name == "control")
    //    //{
    //    //    vector<string> attribs;
    //    //    attribs.push_back("inPosition");
    //    //    attribs.push_back("inTexCoord");
    //    //    attribs.push_back("inModelMatrix");

    //    //    auto program = load(name, attribs);
    //    //    program->addUniform(0, "textureArrays");

    //    //    return program;
    //    //}
    //    //else if (name == "text")
    //    //{
    //    //    vector<string> attribs;
    //    //    attribs.push_back("inPosition");
    //    //    attribs.push_back("inTexCoord");
    //    //    attribs.push_back("inNormal");
    //    //    attribs.push_back("inTangent");
    //    //    attribs.push_back("inGlyphId");
    //    //    attribs.push_back("inModelMatrix");

    //    //    auto program = load(name, attribs);
    //    //    program->addUniform(0, "textureArrays");
    //    //    program->addUniform(1, "texelSize");

    //    //    return program;
    //    //}
    //    //else if (name == "geometryPass")
    //    //{
    //    //    vector<string> gBufferShaderAttribs;
    //    //    gBufferShaderAttribs.push_back("inPosition");
    //    //    gBufferShaderAttribs.push_back("inTexCoord");
    //    //    gBufferShaderAttribs.push_back("inNormal");
    //    //    gBufferShaderAttribs.push_back("inTangent");
    //    //    gBufferShaderAttribs.push_back("inMaterialId");
    //    //    gBufferShaderAttribs.push_back("inModelMatrix");

    //    //    auto program = load(name, gBufferShaderAttribs);
    //    //    //program->addUniform(0, "textureArrays");

    //    //    return program;
    //    //}
    //    //else if (name == "lightingPass")
    //    //{
    //    //    vector<string> lightingShaderAttribs;
    //    //    lightingShaderAttribs.push_back("inPosition");
    //    //    lightingShaderAttribs.push_back("inTexCoord");

    //    //    auto program = load(name, lightingShaderAttribs);
    //    //    program->addUniform(0, "textureArrays");
    //    //    
    //    //    return program;
    //    //}

    //    //return nullptr;
    //}

    //program* shadersManager::load(const string& name)
    //{
    //    auto vertFile = path::combine(_path, name, shadersManager::VERT_EXT);
    //    auto fragFile = path::combine(_path, name, shadersManager::FRAG_EXT);

    //    auto vertexShader = new phi::shader(vertFile);
    //    auto fragmentShader = new phi::shader(fragFile);

    //    auto program = glslCompiler::compile({vertexShader, fragmentShader});

    //    _programs[name] = program;

    //    return program;
    //}

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