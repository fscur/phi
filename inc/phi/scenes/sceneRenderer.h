#ifndef _PHI_SCENE_RENDERER_H_
#define _PHI_SCENE_RENDERER_H_

#include <phi\rendering\defaultFrameBuffer.h>
#include <phi\rendering\pickingFrameBuffer.h>

#include "scenes.h"
#include "scene.h"

namespace phi
{
    class sceneRenderer
    {
    private:
        enum BASIC_SHADER
        {
            MVP,
            TEXTURE_ARRAYS,
            DIFFUSE_COLOR,
            DIFFUSE_TEXTURE_ARRAY_INDEX,
            DIFFUSE_TEXTURE_PAGE_INDEX
        };

    private:

        defaultFrameBuffer* _defaultFrameBuffer;
        scene* _scene;
        camera* _camera;
        shader* _shader;
        bool _hasBindlessTextures;

    private:
        void createShader();

    public:
        SCENES_API sceneRenderer(sizef size);
        SCENES_API ~sceneRenderer();
        
        SCENES_API defaultFrameBuffer* getDefaultFrameBuffer() const { return _defaultFrameBuffer; }
        SCENES_API void init();
        SCENES_API void initExtensions();
        SCENES_API void render(scene* scene);
    };
}

#endif