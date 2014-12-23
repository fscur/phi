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
		RENDERING_API basicSceneRenderer(size<GLuint> viewportSize);
		RENDERING_API ~basicSceneRenderer();
		RENDERING_API void onRender() override;
	};
}

#endif