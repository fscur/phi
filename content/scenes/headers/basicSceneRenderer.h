#ifndef _PHI_BASIC_SCENE_RENDERER_H_
#define _PHI_BASIC_SCENE_RENDERER_H_

#include "sceneRenderer.h"

namespace phi
{
	class basicSceneRenderer : 
		public sceneRenderer
	{
	private:
		texture* _defaultDiffuseMap;

	public:
		SCENES_API basicSceneRenderer(size<GLuint> viewportSize);
		SCENES_API ~basicSceneRenderer();
		SCENES_API void onRender() override;
	};
}

#endif