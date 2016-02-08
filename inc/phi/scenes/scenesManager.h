#ifndef _PHI_SCENES_MANAGER_H
#define _PHI_SCENES_MANAGER_H

#include <phi\scenes\scene.h>
#include <phi\rendering\renderer.h>
#include <phi\core\resourcesRepository.h>

namespace phi
{
    static const GLuint PHI_DEBUG_MODE = 1;
    static const GLuint PHI_SHADOW_MAPS = 101;

    struct scenesManagerInfo
    {
    public:
        std::string applicationPath;
        std::string resourcesPath;
        sizef size;
    };

    class scenesManager
    {
    private:
        static scenesManager* _instance;
        std::vector<scene*> _scenes;
        scene* _scene;
        renderer* _renderer;
        scenesManagerInfo _info;
        bool _isShadowMapEnabled;
        bool _debugMode;

    private:
        scenesManager();

    public:
        SCENES_API ~scenesManager();

        SCENES_API scene* getScene() const { return _scene; }
        SCENES_API scenesManagerInfo getInfo() const { return _info; }
        SCENES_API renderer* getRenderer() const { return _renderer; }
        SCENES_API void setSceneRenderer(renderer* value) { _renderer = value; }

        SCENES_API bool init(scenesManagerInfo info);
        SCENES_API void update();
        SCENES_API void render();
        SCENES_API void release();
        SCENES_API void resize(sizef size);

        SCENES_API void addScene(scene* scene) { _scenes.push_back(scene); }
        SCENES_API void loadScene(GLuint id) { _scene = _scenes[id]; }

        SCENES_API static scenesManager* get();

        SCENES_API void enable(GLuint flags);
        SCENES_API void disable(GLuint flags);
    };
}
#endif