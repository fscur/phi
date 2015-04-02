#ifndef _PHI_FS_SCENE_RENDERER_H_
#define _PHI_FS_SCENE_RENDERER_H_

#include "sceneRenderer.h"
#include "quad.h"

namespace phi
{
	class fsSceneRenderer : 
		public sceneRenderer
	{
	private:
		quad _quad;
		frameBuffer* _frameBuffer;
		frameBuffer* _postFrameBuffer;
        glm::mat4 _projMatrix;
        glm::mat4 _viewMatrix;
        std::map<unsigned int, glm::mat4> _modelMatrices;
        std::map<unsigned int, glm::mat4> _mvpMatrices;
        std::map<unsigned int, glm::mat4> _itmvMatrices;

	private:
		void createDefaultRenderTarget();
		void createSelectedObjectsRenderTarget();
		void createDepthBuffer();
        
		void createPostRenderTarget();

		void createAmbientLightShader();
		void createDirLightShader();
		void createPointLightShader();
		void createSpotLightShader();
        void createEmissiveBloomShaders();

		void ambientLightPass();
		void dirLightPasses();
		void pointLightPasses();
		void spotLightPasses();

		void render();
        void emissiveBloomPass();
		void selectedObjectsPass();

	public:
		SCENES_API fsSceneRenderer(size<GLuint> viewportSize);
		SCENES_API ~fsSceneRenderer();
		SCENES_API void onRender() override;
	};
}

#endif