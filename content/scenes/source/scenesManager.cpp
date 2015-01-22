#include "scenesManager.h"
#include "globals.h"
#include "shaderManager.h"
#include "renderingSystem.h"

namespace phi
{
    scenesManager* scenesManager::_instance = nullptr;
    basicSceneRenderer* scenesManager::basicRenderer = nullptr;
    fsSceneRenderer* scenesManager::fsRenderer = nullptr;
    dsSceneRenderer* scenesManager::dsRenderer = nullptr;

    scenesManager::scenesManager()
    {
        _isShadowMapEnabled = false;
        _debugMode = false;
    }

    scenesManager::~scenesManager()
    {
        for (GLuint i = 0; i < _scenes.size(); i++)
            DELETE(_scenes[i]);

        DELETE(basicRenderer);
        DELETE(fsRenderer);
        DELETE(dsRenderer);
    }

    bool scenesManager::init(scenesManagerInfo info)
    {
        bool result = false;

        _info = info;

        if (!renderingSystem::initialized)
        {
            renderingSystemInfo renderingInfo = renderingSystemInfo();
            renderingInfo.applicationPath = _info.applicationPath;
            renderingInfo.size = info.size;
            renderingSystem::init(renderingInfo);
        }

        basicRenderer = new basicSceneRenderer(_info.size);
        fsRenderer = new fsSceneRenderer(_info.size);
        dsRenderer = new dsSceneRenderer(_info.size);

        setSceneRenderer(scenesManager::basicRenderer);

        if (!result)
            return false;

        return true;
    }

    void scenesManager::handleInput()
    {
        _scene->input();
    }

    void scenesManager::update()
    {
        _scene->update();
    }

    void scenesManager::render()
    {
		 _sceneRenderer->render(_scene);
    }

    void scenesManager::release()
    {
        //TODO: Think about how to handle HUD system, as we need the resource manager and shader manager to display the HUD for now. EDIT: I don't think that's a problem anymore.
        renderingSystem::release();
        shaderManager::get()->release();

        DELETE(_instance);
    }

    void scenesManager::resize(size<GLuint> size)
    {
        _scene->setSize(size);
        renderingSystem::resize(size);
    }

    scenesManager* scenesManager::get()
    {
        if (!_instance)
            _instance = new scenesManager();

        return _instance;
    }

    void scenesManager::enable(GLuint flags)
    {
        if ((flags | PHI_SHADOW_MAPS) == PHI_SHADOW_MAPS)
            _isShadowMapEnabled = true;

        if ((flags | PHI_DEBUG_MODE) == PHI_DEBUG_MODE)
            _debugMode = true;
    }

    void scenesManager::disable(GLuint flags)
    {
        if ((flags | PHI_SHADOW_MAPS) == PHI_SHADOW_MAPS)
            _isShadowMapEnabled = false;

        if ((flags | PHI_DEBUG_MODE) == PHI_DEBUG_MODE)
            _debugMode = false;
    }
}