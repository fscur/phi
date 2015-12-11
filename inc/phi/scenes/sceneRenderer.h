#ifndef _PHI_SCENE_RENDERER_H_
#define _PHI_SCENE_RENDERER_H_

#include "scenes.h"
#include "scene.h"

#include <phi\rendering\defaultFrameBuffer.h>
#include <phi\rendering\pickingFrameBuffer.h>

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

        defaultFrameBuffer* _defaultFrameBuffer;
        scene* _scene;
        camera* _camera;
        shader* _shader;

    private:
        void createShader();

    public:
        SCENES_API sceneRenderer();
        SCENES_API ~sceneRenderer();
        
        SCENES_API defaultFrameBuffer* getDefaultFrameBuffer() const { return _defaultFrameBuffer; }
        SCENES_API void init();
        SCENES_API void render(scene* scene);
    };
}

#endif