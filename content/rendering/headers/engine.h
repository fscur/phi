#ifndef _PHI_ENGINE_H
#define _PHI_ENGINE_H

#include "scene.h"
#include "basicSceneRenderer.h"
#include "fsSceneRenderer.h"
#include "dsSceneRenderer.h"

namespace phi
{
    const static const GLuint PHI_DEBUG_MODE = 1;
    const static const GLuint PHI_SHADOW_MAPS = 101;

    class engine
    {
    private:
        static engine* _instance;
		std::vector<scene*> _scenes;
        scene* _scene;
		sceneRenderer* _sceneRenderer;

        bool _isShadowMapEnabled;
        bool _debugMode;

	public:
		RENDERING_API static basicSceneRenderer* basicRenderer;
		RENDERING_API static fsSceneRenderer* fsRenderer;
		RENDERING_API static dsSceneRenderer* dsRenderer;

    private:
        engine();

    public:
        RENDERING_API ~engine();
    
		RENDERING_API scene* getScene() const { return _scene; }
		RENDERING_API void setSceneRenderer(sceneRenderer* value) { _sceneRenderer = value; }

        RENDERING_API bool init(size<GLuint> size);
        RENDERING_API void handleInput();
        RENDERING_API void update();
        RENDERING_API void render();
        RENDERING_API void release();
		RENDERING_API void resize(size<GLuint> size);

		RENDERING_API void addScene(scene* scene) { _scenes.push_back(scene); }
		RENDERING_API void loadScene(GLuint id) { _scene = _scenes[id]; }

        RENDERING_API static engine* get();

        RENDERING_API void enable(GLuint flags);
        RENDERING_API void disable(GLuint flags);
    };
}
#endif