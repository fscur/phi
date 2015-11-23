#ifndef _PHI_SCENES_MANAGER_H
#define _PHI_SCENES_MANAGER_H

#include "phi/scenes/scene.h"
#include "phi/scenes/basicSceneRenderer.h"
#include "phi/scenes/fsSceneRenderer.h"
#include "phi/scenes/dsSceneRenderer.h"
#include "phi/core/resourcesRepository.h"

namespace phi
{
    static const GLuint PHI_DEBUG_MODE = 1;
    static const GLuint PHI_SHADOW_MAPS = 101;

	struct scenesManagerInfo
	{
	public:
		std::string applicationPath;
		size<GLuint> size;
	};

	class scenesManager
    {
    private:
        static scenesManager* _instance;
		std::vector<scene*> _scenes;
        scene* _scene;
		sceneRenderer* _sceneRenderer;
		scenesManagerInfo _info;
        bool _isShadowMapEnabled;
        bool _debugMode;

	public:
		SCENES_API static basicSceneRenderer* basicRenderer;
		SCENES_API static fsSceneRenderer* fsRenderer;
		SCENES_API static dsSceneRenderer* dsRenderer;

    private:
        scenesManager();

    public:
        SCENES_API ~scenesManager();
    
		SCENES_API scene* getScene() const { return _scene; }
		SCENES_API scenesManagerInfo getInfo() const { return _info; }
        SCENES_API sceneRenderer* getSceneRenderer() const { return _sceneRenderer; }
        SCENES_API void setSceneRenderer(sceneRenderer* value) { _sceneRenderer = value; }

        SCENES_API bool init(scenesManagerInfo info);
        SCENES_API void handleInput();
        SCENES_API void update();
        SCENES_API void render();
        SCENES_API void release();
		SCENES_API void resize(size<GLuint> size);

		SCENES_API void addScene(scene* scene) { _scenes.push_back(scene); }
		SCENES_API void loadScene(GLuint id) { _scene = _scenes[id]; }

        SCENES_API static scenesManager* get();

        SCENES_API void enable(GLuint flags);
        SCENES_API void disable(GLuint flags);
    };
}
#endif