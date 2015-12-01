#ifndef _PHI_SCENE_RENDERER_H_
#define _PHI_SCENE_RENDERER_H_

#include "phi/scenes/scenes.h"
#include "phi/scenes/scene.h"
#include "phi/rendering/renderingSystem.h"

namespace phi
{
    class sceneRenderer
    {
    protected:
        scene* _scene;
        std::vector<sceneObject*>* _allObjects;
        GLuint _allObjectsCount;
        size<GLuint> _viewportSize;
        camera* _camera;
        bool _redrawStaticShadowMaps;

    protected:
        SCENES_API virtual void onRender() = 0;
        SCENES_API virtual void init(size<GLuint> viewportSize) = 0;
        SCENES_API virtual void initBuffers() = 0;
    public:
        SCENES_API sceneRenderer();
        SCENES_API ~sceneRenderer();
        SCENES_API void render(scene* scene);
        SCENES_API color getSelectionColor(int objectId, int meshId, bool selected);
        SCENES_API void resize(size<GLuint> size);
        SCENES_API void redrawStaticShadowMaps() { _redrawStaticShadowMaps = true; };
    };
}

#endif