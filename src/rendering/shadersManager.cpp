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