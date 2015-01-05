#include "sceneRenderer.h"

namespace phi
{
	sceneRenderer::sceneRenderer(size<GLuint> viewportSize)
	{
		_viewportSize = viewportSize;
	}

	sceneRenderer::~sceneRenderer()
	{
	}

	void sceneRenderer::render(scene* scene)
	{
		_scene = scene;
		_allObjects = _scene->getAllObjects();
		_allObjectsCount = _allObjects->size();
		_camera = _scene->getActiveCamera();

		onRender();
	}
}