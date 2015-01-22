#ifndef _PHI_DS_SCENE_RENDERER_H_
#define _PHI_DS_SCENE_RENDERER_H_

#include "sceneRenderer.h"
#include "quad.h"

namespace phi
{
	class dsSceneRenderer : 
		public sceneRenderer
	{
	private:
		frameBuffer* _frameBuffer;
		quad _quad;
		bool _hasSelectedObjects;
	private:
		void createDefaultRenderTarget();
		void createPositionRenderTarget();
		void createNormalRenderTarget();
		void createDiffuseRenderTarget();
		void createSpecularRenderTarget();
		void createShininessRenderTarget();
		void createSelectedObjectsRenderTarget();
		void createDepthBuffer();

		void createGeomPassShader();
		void createStencilShader();
		void createDirLightShader();
		void createPointLightShader();
		void createSpotLightShader();

		void geomPass();
		void directionalLightPass();
		void pointLightPass();
		void spotLightPass();
		void selectedObjectsPass();
		void render();

	public:
		SCENES_API dsSceneRenderer(size<GLuint> viewportSize);
		SCENES_API ~dsSceneRenderer();
		SCENES_API void onRender() override;
	};
}

#endif