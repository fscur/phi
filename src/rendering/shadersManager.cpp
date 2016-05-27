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

            auto gBufferShader = load(name, gBufferShaderAttribs);
            gBufferShader->addUniform(0, "textureArrays");

            return gBufferShader;
        }
        else if (name == "lightingPass")
        {
            vector<string> lightingShaderAttribs;
            lightingShaderAttribs.push_back("inPosition");
            lightingShaderAttribs.push_back("inTexCoord");

            auto lightingShader = load(name, lightingShaderAttribs);
            lightingShader->addUniform(0, "textureArrays");

            return lightingShader;
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

    shader* shadersManager::get(string name)
    {
        if (_shaders.find(name) == _shaders.end())
            return nullptr;
        else
            return _shaders[name];
    }

    void shadersManager::reloadAllShaders()
    {
        for (auto& pair : _shaders)
        {
            auto shader = pair.second;
            while (!shader->reload())
            {
                phi::debug("\nFailed \"" + pair.first + "\" shader compilation.\nPress enter to try again...\n");
                std::cin.get();
            }
        }
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