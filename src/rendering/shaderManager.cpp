#include <phi/rendering/shaderManager.h>

namespace phi
{
    shaderManager* shaderManager::_instance = nullptr;

    shaderManager::shaderManager()
    {
    }

    shaderManager::~shaderManager()
    {
    }

    shaderManager* shaderManager::get()
    {
        if (!_instance)
            _instance = new shaderManager();

        return _instance;
    }

    void shaderManager::init(shaderManagerInfo info)
    {
        _info = info;
    }

    void shaderManager::addShader(shader* shader)
    {
        if (_shaders.find(shader->getId()) != _shaders.end())
            return;

        _shaders[shader->getId()] = shader;
    }

    shader* shaderManager::loadShader(std::string vertFile, std::string fragFile, std::vector<std::string> attributes)
    {
        shader* s = new shader(_info.path + SHADERS_PATH + vertFile, _info.path + SHADERS_PATH + fragFile, attributes);
        s->init();
        return s;
    }

    shader* shaderManager::getShader(GLuint id)
    {
        if (_shaders.find(id) == _shaders.end())
            return nullptr;
        else
            return _shaders[id];
    }

    void shaderManager::release()
    {
        for(auto pair : _shaders) 
        {
            pair.second->release();
            safeDelete(pair.second);
        }

        if (_instance)
            safeDelete(_instance);
    }
}