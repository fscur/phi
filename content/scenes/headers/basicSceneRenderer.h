#ifndef _PHI_BASIC_SCENE_RENDERER_H_
#define _PHI_BASIC_SCENE_RENDERER_H_

#include "sceneRenderer.h"
#include "quad.h"

namespace phi
{
	class basicSceneRenderer : 
		public sceneRenderer
	{
	private:
		quad _quad;
		texture* _defaultDiffuseMap;
		frameBuffer* _frameBuffer;
	
	private:
		void createDefaultRenderTarget();
		void createSelectedObjectsRenderTarget();
		void createDepthBuffer();
		void createGeomPassShader();
		void render();
		void selectedObjectsPass();
	public:
		SCENES_API basicSceneRenderer(size<GLuint> viewportSize);
		SCENES_API ~basicSceneRenderer();
		SCENES_API void onRender() override;
	};
}

#endif