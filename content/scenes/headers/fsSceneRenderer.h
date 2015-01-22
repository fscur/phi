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

	private:
		void createDefaultRenderTarget();
		void createSelectedObjectsRenderTarget();
		void createDepthBuffer();

		void createAmbientLightShader();
		void createDirLightShader();
		void createPointLightShader();
		void createSpotLightShader();

		void ambientLightPass();
		void dirLightPasses();
		void pointLightPasses();
		void spotLightPasses();

		void render();
		void selectedObjectsPass();

	public:
		SCENES_API fsSceneRenderer(size<GLuint> viewportSize);
		SCENES_API ~fsSceneRenderer();
		SCENES_API void onRender() override;
	};
}

#endif