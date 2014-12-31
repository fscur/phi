#include "engine.h"
#include "globals.h"
#include "shaderManager.h"

namespace phi
{
	engine* engine::_instance = nullptr;
	basicSceneRenderer* engine::basicRenderer = nullptr;
	fsSceneRenderer* engine::fsRenderer = nullptr;
	dsSceneRenderer* engine::dsRenderer = nullptr;

    engine::engine()
    {
		_isShadowMapEnabled = false;
		_debugMode = false;
    }

    engine::~engine()
    {
		for (GLuint i = 0; i < _scenes.size(); i++)
			DELETE(_scenes[i]);

		DELETE(basicRenderer);
		DELETE(fsRenderer);
		DELETE(dsRenderer);
    }

	bool engine::init(engineInfo info)
    {
        bool result = false;

		_info = info;

        if (!rendering::initialized)
            rendering::init(_info.applicationPath);

		basicRenderer = new phi::basicSceneRenderer(_info.size);
		fsRenderer = new phi::fsSceneRenderer(_info.size);
		dsRenderer = new phi::dsSceneRenderer(_info.size);

		setSceneRenderer(phi::engine::basicRenderer);

		shaderManagerInfo shaderInfo;
		shaderInfo.path = info.applicationPath;
		shaderManager::get()->init(shaderInfo);

        if (!result)
            return false;

        return true;
    }

    void engine::handleInput()
    {
        _scene->input();
    }

    void engine::update()
    {
        _scene->update();
    }

    void engine::render()
    {
		_sceneRenderer->render(_scene);
    }

    void engine::release()
    {
        //TODO: Think about how to handle HUD system, as we need the resource manager and shader manager to display the HUD for now
        rendering::release();
        shaderManager::get()->release();

        DELETE(_instance);
    }

	void engine::resize(size<GLuint> size)
	{
		_scene->setSize(size);
		basicRenderer->resize(size);
		fsRenderer->resize(size);
		dsRenderer->resize(size);
		_sceneRenderer->resize(size);
	}

    engine* engine::get()
    {
        if (!_instance)
            _instance = new engine();

        return _instance;
    }
	
	void engine::enable(GLuint flags)
	{
		if ((flags | PHI_SHADOW_MAPS) == PHI_SHADOW_MAPS)
			_isShadowMapEnabled = true;

		if ((flags | PHI_DEBUG_MODE) == PHI_DEBUG_MODE)
			_debugMode = true;
	}

	void engine::disable(GLuint flags)
	{
		if ((flags | PHI_SHADOW_MAPS) == PHI_SHADOW_MAPS)
			_isShadowMapEnabled = false;

		if ((flags | PHI_DEBUG_MODE) == PHI_DEBUG_MODE)
			_debugMode = false;
	}
}