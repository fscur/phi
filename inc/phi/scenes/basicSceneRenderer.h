#ifndef _PHI_BASIC_SCENE_RENDERER_H_
#define _PHI_BASIC_SCENE_RENDERER_H_

#include "phi/rendering/quad.h"

#include "phi/scenes/sceneRenderer.h"

namespace phi
{
    class basicSceneRenderer :
        public sceneRenderer
    {
    private:
        quad* _quad;
        frameBuffer* _frameBuffer;

    private:
        void createDefaultRenderTarget();
        void createSelectedObjectsRenderTarget();
        void createDepthBuffer();
        void createGeomPassShader();
        void render();
        void selectedObjectsPass();
    public:
        SCENES_API basicSceneRenderer();
        SCENES_API ~basicSceneRenderer();
        SCENES_API void onRender() override;
        SCENES_API void initBuffers() override;
        SCENES_API void init(size<GLuint> viewportSize) override;
    };
}

#endif