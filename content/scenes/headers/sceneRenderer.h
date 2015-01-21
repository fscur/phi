#ifndef _PHI_SCENE_RENDERER_H_
#define _PHI_SCENE_RENDERER_H_

#include "scenes.h"
#include "scene.h"
#include "renderingSystem.h"

namespace phi
{
	class sceneRenderer
	{
	protected:
		scene* _scene;
		std::vector<sceneObject*>* _allObjects;
		GLuint _allObjectsCount;
		size<GLuint> _viewportSize;
		camera* _camera;
		
	protected:
		SCENES_API virtual void onRender() = 0;
	public:
		SCENES_API sceneRenderer(size<GLuint> viewportSize);
		SCENES_API ~sceneRenderer();
		SCENES_API void render(scene* scene);
	};
}

#endif