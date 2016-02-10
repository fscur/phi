#include "scenesManager.h"

#include <core\globals.h>

#include <io\path.h>

#include <rendering\renderingSystem.h>
#include <rendering\shaderManager.h>

namespace phi
{
    scenesManager* scenesManager::_instance = nullptr;

    scenesManager::scenesManager()
    {
        _isShadowMapEnabled = false;
        _debugMode = false;
    }

    scenesManager::~scenesManager()
    {
        for(GLuint i = 0; i < _scenes.size(); i++)
            safeDelete(_scenes[i]);
    }

    bool scenesManager::init(scenesManagerInfo info)
    {
        bool result = false;

        _info = info;

        if(!renderingSystem::initialized)
        {
            renderingSystemInfo renderingInfo = renderingSystemInfo();
            renderingInfo.applicationPath = _info.applicationPath;
            renderingInfo.resourcesPath = _info.resourcesPath;
            renderingInfo.size = info.size;
            renderingSystem::init(renderingInfo);
        }

        if(!result)
            return false;

        return true;
    }

    void scenesManager::update()
    {
        _scene->update();
    }

    void scenesManager::render()
    {
        _renderer->render();
    }

    void scenesManager::release()
    {
        renderingSystem::release();
        shaderManager::get()->release();

        safeDelete(_instance);
    }

    void scenesManager::resize(sizef size)
    {
        _scene->setSize(size);
        renderingSystem::resize(size);
    }

    scenesManager* scenesManager::get()
    {
        if(!_instance)
            _instance = new scenesManager();

        return _instance;
    }

    void scenesManager::enable(GLuint flags)
    {
        if((flags | PHI_SHADOW_MAPS) == PHI_SHADOW_MAPS)
            _isShadowMapEnabled = true;

        if((flags | PHI_DEBUG_MODE) == PHI_DEBUG_MODE)
            _debugMode = true;
    }

    void scenesManager::disable(GLuint flags)
    {
        if((flags | PHI_SHADOW_MAPS) == PHI_SHADOW_MAPS)
            _isShadowMapEnabled = false;

        if((flags | PHI_DEBUG_MODE) == PHI_DEBUG_MODE)
            _debugMode = false;
    }
}