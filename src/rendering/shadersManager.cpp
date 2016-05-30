#include <precompiled.h>
#include "shadersManager.h"

#include <diagnostics\stopwatch.h>

#include <io\path.h>

namespace phi
{
    shadersManager::shadersManager(string path) :
        _path(path)
    {
    }

    shadersManager::~shadersManager()
    {
        for (auto& pair : _shaders)
            safeDelete(pair.second);
    }

    shader * shadersManager::loadCrazyFuckerSpecificShader(const string& name)
    {
        if (name == "control")
        {
            vector<string> attribs;
            attribs.push_back("inPosition");
            attribs.push_back("inTexCoord");
            attribs.push_back("inModelMatrix");

            auto shader = load(name, attribs);
            shader->addUniform(0, "textureArrays");

            return shader;
        }
        else if (name == "text")
        {
            vector<string> attribs;
            attribs.push_back("inPosition");
            attribs.push_back("inTexCoord");
            attribs.push_back("inNormal");
            attribs.push_back("inTangent");
            attribs.push_back("inGlyphId");
            attribs.push_back("inModelMatrix");

            auto shader = load(name, attribs);
            shader->addUniform(0, "textureArrays");
            shader->addUniform(1, "texelSize");

            return shader;
        }
        else if (name == "geometryPass")
        {
            vector<string> gBufferShaderAttribs;
            gBufferShaderAttribs.push_back("inPosition");
            gBufferShaderAttribs.push_back("inTexCoord");
            gBufferShaderAttribs.push_back("inNormal");
            gBufferShaderAttribs.push_back("inTangent");
            gBufferShaderAttribs.push_back("inMaterialId");
            gBufferShaderAttribs.push_back("inModelMatrix");

            auto shader = load(name, gBufferShaderAttribs);
            shader->addUniform(0, "textureArrays");

            return shader;
        }
        else if (name == "lightingPass")
        {
            vector<string> lightingShaderAttribs;
            lightingShaderAttribs.push_back("inPosition");
            lightingShaderAttribs.push_back("inTexCoord");

            auto shader = load(name, lightingShaderAttribs);
            shader->addUniform(0, "textureArrays");

            return shader;
        }

        return nullptr;
    }

    shader* shadersManager::load(string name, const vector<string>& attributes)
    {
        auto vertFile = path::combine(_path, name, shadersManager::VERT_EXT);
        auto fragFile = path::combine(_path, name, shadersManager::FRAG_EXT);

        auto shader = new phi::shader(vertFile, fragFile, attributes);
        shader->init();
        _shaders[name] = shader;

        return shader;
    }

    void shadersManager::reloadShader(string shaderName)
    {
        auto selectedShader = _shaders[shaderName];
        if (selectedShader)
        {
            if (selectedShader->canCompileShader())
            {
                selectedShader->reload();
                debug("<shadersManager> " + shaderName + " reloaded");
            }
            else
            {
                debug("<shaderManager> shader validation error");
            }
        }
        else
        {
            debug("<shadersManager>: shader not found");
        }
    }
}