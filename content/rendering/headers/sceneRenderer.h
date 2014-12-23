#ifndef _PHI_SCENE_RENDERER_H_
#define _PHI_SCENE_RENDERER_H_

#include "rendering.h"
#include "scene.h"

namespace phi
{
	class sceneRenderer
	{
	protected:
		scene* _scene;
		defaultRenderTarget* _defaultRenderTarget;
		std::vector<sceneObject*>* _allObjects;
		GLuint _allObjectsCount;
		size<GLuint> _viewportSize;
		camera* _camera;
		RENDERING_API virtual void onRender() = 0;

	public:
		RENDERING_API sceneRenderer(size<GLuint> viewportSize);
		RENDERING_API ~sceneRenderer();
		RENDERING_API void render(scene* scene);
		RENDERING_API void resize(size<GLuint> viewportSize);
	};
}

#endif