#include "basicSceneRenderer.h"

namespace phi
{
	basicSceneRenderer::basicSceneRenderer(size<GLuint> viewportSize) : sceneRenderer(viewportSize),
		_defaultDiffuseMap(rendering::repository->getResource<texture>("default_diffuseMap"))
	{
	}
	
	basicSceneRenderer::~basicSceneRenderer()
	{
	}

	void basicSceneRenderer::onRender()
	{
		shader* sh = shaderManager::get()->getShader("BASIC");
		sh->bind();

		for (GLuint i = 0; i < _allObjectsCount; i++)
		{
			sceneObject* sceneObj = (*_allObjects)[i];

			sh->setUniform("mvp", sceneObj->getTransform()->getMvp());

			if (sceneObj->getMaterial() == nullptr)
			{
				sh->setUniform("diffuseMap", _defaultDiffuseMap);
				sh->setUniform("diffuseColor", color::white);
			}
			else
			{
				sh->setUniform("diffuseMap", sceneObj->getMaterial()->getDiffuseTexture());
				sh->setUniform("diffuseColor", sceneObj->getMaterial()->getDiffuseColor());
			}

			sceneObj->render();
		}

		sh->unbind();
	}
}