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

        std::vector<frameBuffer*> _dirLightShadowMapFrameBuffers0;
        std::vector<frameBuffer*> _dirLightShadowMapFrameBuffers1;
        std::vector<frameBuffer*> _spotLightShadowMapFrameBuffers0;
        std::vector<frameBuffer*> _spotLightShadowMapFrameBuffers1;

        unsigned int _shadowMapSize;
        unsigned int _pointLightShadowMapSize;

        float _nearPlane;
        float _farPlane;
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

        texture* _randomNormalsTexture;
        texture* _noiseTexture;

	    bool _ssaoActive;
        float _ssaoBias;
        float _ssaoScale;
        float _ssaoSampleRadius;
        float _ssaoIntensity;

        std::map<GLuint, sceneObject*> _staticObjects;
        std::map<GLuint, sceneObject*> _dynamicObjects;

        texture* _rt0Texture;
        texture* _rt1Texture;
        texture* _rt2Texture;
        texture* _rt3Texture;

        shader* _shadowMapShader;
        shader* _pointLightShadowMapShader;
        shader* _geometryPassShader;
        shader* _stencilShader;
        shader* _dirLightShader;
        shader* _spotLightShader;
        shader* _pointLightShader;
        shader* _ssaoShader;
        shader* _reflectionsShader;
        shader* _blurShader;

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
        void shadowMapPasses();
        void staticShadowMapPass(frameBuffer* staticFrameBuffer, glm::mat4 l, float n, float f);
        void dynamicShadowMapPass(frameBuffer* staticFrameBuffer, frameBuffer* dynamicFrameBuffer, glm::mat4 l, float n, float f);
        void directionalLightPass();
        void pointLightPass();
        void spotLightPass();
        void selectedObjectsPass();
        void ssao();

        texture* blur(texture* source);

        void render();

        void createDirLightShadowMapsRenderTargets();
        void createSpotLightShadowMapsRenderTargets();

    public:
        dsSceneRendererEx(size<GLuint> viewportSize);
        ~dsSceneRendererEx();
        void onRender() override;
        void initBuffers() override;
    };
}

#endif