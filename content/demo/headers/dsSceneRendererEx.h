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
        std::vector<frameBuffer*> _dirLightShadowMapFrameBuffers;

        unsigned int _shadowMapSize;
        unsigned int _pointLightShadowMapSize;
        glm::vec3 _pointLightShadowMapDirs[6];
        glm::vec3 _pointLightShadowMapUps[6];

        quad _quad;
        bool _hasSelectedObjects;
        bool _buffersInitialized;

        glm::mat4 _projMatrix;
        glm::mat4 _viewMatrix;
        glm::mat4 _biasMatrix;

        std::map<unsigned int, glm::mat4> _modelMatrices;
        std::map<unsigned int, glm::mat4> _mvpMatrices;
        std::map<unsigned int, glm::mat4> _itmvMatrices;

        std::map<directionalLight*, texture*> _dirLightShadowMaps;

        texture* _randomNormalsTexture;
        texture* _noiseTexture;

	    bool _ssaoActive;
        float _ssaoBias;
        float _ssaoScale;
        float _ssaoSampleRadius;
        float _ssaoIntensity;

    private:

        void createGeometryPassRenderTargets();
        void createShadowMapsRenderTargets();
        void createPointLightShadowMapRenderTargets();
        void createFinalImageRenderTargets();

        void createGeomPassShader();
        void createStencilShader();
        void createDirLightShader();
        void createPointLightShader();
        void createSpotLightShader();

        void createShadowMapShaders();

        void createSSAOShader();

        void geomPass();
        void directionalLightPass();
        void pointLightPass();
        void spotLightPass();
        void selectedObjectsPass();
        void ssao();

        texture* blur(texture* source);

        void render();

        void createDirLightShadowMapsRenderTargets();

    public:
        dsSceneRendererEx(size<GLuint> viewportSize);
        ~dsSceneRendererEx();
        void onRender() override;
        void initBuffers() override;
    };
}

#endif