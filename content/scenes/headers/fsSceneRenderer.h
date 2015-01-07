#ifndef _PHI_FS_SCENE_RENDERER_H_
#define _PHI_FS_SCENE_RENDERER_H_

#include "sceneRenderer.h"

namespace phi
{
	class fsSceneRenderer : 
		public sceneRenderer
	{
	private:
		void fsAmbientLightPass();
		void fsDirLightPasses();
		void fsPointLightPasses();
		void fsSpotLightPasses();

	public:
		SCENES_API fsSceneRenderer(size<GLuint> viewportSize);
		SCENES_API ~fsSceneRenderer();
		SCENES_API void onRender() override;
	};
}

#endif