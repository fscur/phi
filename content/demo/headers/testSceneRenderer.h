#ifndef _TEST_SCENE_RENDERER_H_
#define _TEST_SCENE_RENDERER_H_

#include "sceneRenderer.h"
#include "quad.h"
#include "skyDome.h"

namespace phi
{
	class testSceneRenderer : 
		public sceneRenderer
	{
	private:
		quad _quad;
		std::vector<frameBuffer*> _frameBuffers;
        texture* _randomNormalsTexture;
        texture* _noiseTexture;

	    bool _ssaoActive;
        float _ssaoBias;
        float _ssaoScale;
        float _ssaoSampleRadius;
        float _ssaoIntensity;
        size<GLuint> _shadowMapSize;
        glm::mat4 _projMatrix;
        glm::mat4 _viewMatrix;
        std::map<unsigned int, glm::mat4> _modelMatrices;
        std::map<unsigned int, glm::mat4> _mvpMatrices;
        std::map<unsigned int, glm::mat4> _itmvMatrices;
        skyDome* _skyDome;
        bool _hasSelectedObjects;

	private:
		void createDefaultRenderTargets();
        void createShadowMapsRenderTargets();
        void createPointLightShadowMapRenderTargets();
        void createSelectedObjectsRenderTarget();
		void createFXRenderTargets();
		void createGeomPassShader();
		void createDirLightShaders();
		void createPointLightShader();
		void createSpotLightShader();
        void createShadowMapsShaders();
		void createSSAOShader();
		void createReflectionsShader();
        void createRefractionsShader();
        void createBlurShader();
        void createSkyDomeShader();
        void geomPass();
        void dirLightPass(directionalLight* light);
        void dirLightPasses();
        void pointLightPasses();
        void spotLightPasses();
        void ssao();
        void reflections();
        void refractions();
        void renderSkyDome();
        texture* blur(texture* tex);
        void selectedObjectsPass();

	public:
		testSceneRenderer(size<GLuint> viewportSize);
		~testSceneRenderer();
		void onRender() override;
        void initBuffers() override;
        void resize(size<GLuint> size);
        
        void setSSAOActive(bool value) { _ssaoActive = value; }
        void setSSAOBias(float value) { _ssaoBias = value; }
        void setSSAOScale(float value) { _ssaoScale = value; }
        void setSSAOSampleRadius(float value) { _ssaoSampleRadius = value; }
        void setSSAOIntensity(float value) { _ssaoIntensity = value; }
        void setDayTime(float value) { _skyDome->setDayTime(value); }
        void setSunOrientation(float value) { _skyDome->setSunOrientation(value); }
	};
}

#endif