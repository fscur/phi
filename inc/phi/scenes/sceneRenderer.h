#ifndef _PHI_SCENE_RENDERER_H_
#define _PHI_SCENE_RENDERER_H_

#include "phi/scenes/scenes.h"
#include "phi/scenes/scene.h"
#include "phi/rendering/renderingSystem.h"

namespace phi
{
    class sceneRenderer
    {
    private:
        enum BASIC_SHADER
        {
            MVP,
            DIFFUSE_MAP,
            DIFFUSE_COLOR,
        };

    private:
        scene* _scene;
        sizef _viewportSize;
        camera* _camera;
        shader* _shader;

    private:
        void createShader();

    protected:
        SCENES_API virtual void onRender() = 0;
        SCENES_API virtual void init(sizef viewportSize) = 0;
        SCENES_API virtual void initBuffers() = 0;
    public:
        SCENES_API sceneRenderer();
        SCENES_API ~sceneRenderer();
        SCENES_API void render(scene* scene);
        SCENES_API void resize(sizef size);
    };
}

#endif