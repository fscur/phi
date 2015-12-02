#include "phi/scenes/scenesManager.h"

#include "phi/core/globals.h"

#include "phi/rendering/renderingSystem.h"
#include "phi/rendering/shaderManager.h"

#include "phi/io/path.h"

namespace phi
{
    scenesManager* scenesManager::_instance = nullptr;
    basicSceneRenderer* scenesManager::basicRenderer = nullptr;
    fsSceneRenderer* scenesManager::fsRenderer = nullptr;
    dsSceneRenderer* scenesManager::dsRenderer = nullptr;

    scenesManager::scenesManager()
    {
        basicRenderer = new basicSceneRenderer();
        fsRenderer = new fsSceneRenderer();
        dsRenderer = new dsSceneRenderer();

        _isShadowMapEnabled = false;
        _debugMode = false;
    }

    scenesManager::~scenesManager()
    {
        for (GLuint i = 0; i < _scenes.size(); i++)
            safeDelete(_scenes[i]);

        safeDelete(basicRenderer);
        safeDelete(fsRenderer);
        safeDelete(dsRenderer);
    }

    bool scenesManager::init(scenesManagerInfo info)
    {
        bool result = false;

        _info = info;

        if (!renderingSystem::initialized)
        {
            renderingSystemInfo renderingInfo = renderingSystemInfo();
            renderingInfo.applicationPath = _info.applicationPath;
            renderingInfo.resourcesPath = _info.resourcesPath;
            renderingInfo.size = info.size;
            renderingSystem::init(renderingInfo);
        }

        basicRenderer->init(info.size);
        fsRenderer->init(info.size);
        dsRenderer->init(info.size, info.applicationPath);

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
        renderingSystem::release();
        shaderManager::get()->release();

        safeDelete(_instance);
    }

    void scenesManager::resize(size<GLuint> size)
    {
        _scene->setSize(size);
        renderingSystem::resize(size);
        basicRenderer->resize(size);
        fsRenderer->resize(size);
        dsRenderer->resize(size);
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