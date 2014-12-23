#include "sceneRenderer.h"

namespace phi
{
	sceneRenderer::sceneRenderer(size<GLuint> viewportSize)
	{
		_viewportSize = viewportSize;
		_defaultRenderTarget = new defaultRenderTarget(_viewportSize, color::black);
		_defaultRenderTarget->init();
		_defaultRenderTarget->setViewport(0, 0, _viewportSize);
		_defaultRenderTarget->bind();
	}

	sceneRenderer::~sceneRenderer()
	{
		if (_defaultRenderTarget)
			DELETE(_defaultRenderTarget);
	}

	void sceneRenderer::render(scene* scene)
	{
		_scene = scene;
		_allObjects = _scene->getAllObjects();
		_allObjectsCount = _allObjects->size();
		_camera = _scene->getActiveCamera();
		_defaultRenderTarget->clear();

		onRender();
	}

	void sceneRenderer::resize(size<GLuint> viewportSize)
	{
		_defaultRenderTarget->setSize(viewportSize);
		_defaultRenderTarget->setViewport(0, 0, viewportSize);
	}
}