#ifndef _PHI_DS_SCENE_RENDERER_EX_H_
#define _PHI_DS_SCENE_RENDERER_EX_H_

#include <map>
#include "sceneRenderer.h"
#include "quad.h"

namespace phi
{
    class dsSceneRendererEx : 
        public sceneRenderer
    {
    private:
        
		std::vector<frameBuffer*> _frameBuffers;
        quad _quad;
        bool _hasSelectedObjects;

        glm::mat4 _projMatrix;
        glm::mat4 _viewMatrix;
        std::map<unsigned int, glm::mat4> _modelMatrices;
        std::map<unsigned int, glm::mat4> _mvpMatrices;
        std::map<unsigned int, glm::mat4> _itmvMatrices;

        std::map<directionalLight*, texture*> _dirLightShadowMaps;

    private:

        void createRT0();
        void createRT1();
        void createRT2();
        void createRT3();
        void createRT4();
        
        void createShadowMapsRenderTargets();

        void createGeomPassShader();
        void createStencilShader();
        void createDirLightShader();
        void createPointLightShader();
        void createSpotLightShader();

        void createShadowMapShader();

        void geomPass();
        void directionalLightPass();
        void pointLightPass();
        void spotLightPass();
        void selectedObjectsPass();
        void render();

    public:
        SCENES_API dsSceneRendererEx(size<GLuint> viewportSize);
        SCENES_API ~dsSceneRendererEx();
        SCENES_API void onRender() override;
        SCENES_API void initBuffers() override;
    };
}

#endif