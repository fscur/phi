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
		gBuffer* _gBuffer;
		quad _quad;

	private:
		void dsGeomPass();
		void dsAmbientLightBlit();
		void dsDirectionalLightPass();
		void dsPointLightPass();
		void dsSpotLightPass();
		void dsFinalBlit();

	public:
		SCENES_API dsSceneRenderer(size<GLuint> viewportSize);
		SCENES_API ~dsSceneRenderer();
		SCENES_API void onRender() override;
	};
}

#endif