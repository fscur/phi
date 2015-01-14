#include "fsSceneRenderer.h"

namespace phi
{
	fsSceneRenderer::fsSceneRenderer(size<GLuint> viewportSize) : sceneRenderer(viewportSize)
	{
	}

	fsSceneRenderer::~fsSceneRenderer()
	{
	}

	void fsSceneRenderer::onRender()
	{
		//phi::renderingSystem::mainRenderTarget->setViewport(0, 0, phi::size<GLuint>(_viewportSize.width / 2, _viewportSize.height));
		fsAmbientLightPass();
		fsDirLightPasses();
		fsPointLightPasses();
		fsSpotLightPasses();

		auto directionalLights = _scene->getDirectionalLights();
		auto directionalLightsCount = directionalLights->size();

		if (directionalLightsCount == 0)
			return;
	}

	void fsSceneRenderer::fsAmbientLightPass()
	{
		shader* sh = shaderManager::get()->getShader("FS_AMBIENT_LIGHT");
		sh->bind();

		for (GLuint i = 0; i < _allObjectsCount; i++)
		{
			sceneObject* sceneObj = (*_allObjects)[i];

			if (sceneObj->getMaterial() == nullptr)
				continue;

			sh->setUniform("mvp", sceneObj->getTransform()->getMvp());
			sh->setUniform("ambientLightColor", _scene->getAmbientColor());
			sh->setUniform("diffuseMap", sceneObj->getMaterial()->getDiffuseTexture());
			sh->setUniform("mat.ambientColor", sceneObj->getMaterial()->getAmbientColor());
			sh->setUniform("mat.ka", sceneObj->getMaterial()->getKa());

			sceneObj->render();
		}

		sh->unbind();
	}

	void fsSceneRenderer::fsDirLightPasses()
	{
		auto directionalLights = _scene->getDirectionalLights();
		auto directionalLightsCount = directionalLights->size();

		if (directionalLightsCount == 0)
			return;

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		for (GLuint l = 0; l< directionalLightsCount; l++)
		{
			directionalLight* light = (*directionalLights)[l];

			shader* sh = shaderManager::get()->getShader("FS_DIR_LIGHT");

			sh->bind();

			glDepthMask(false);
			glDepthFunc(GL_EQUAL);

			for (GLuint i = 0; i < _allObjectsCount; i++)
			{
				sceneObject* sceneObj = (*_allObjects)[i];

				sh->setUniform("p", sceneObj->getTransform()->getProjectionMatrix());
				sh->setUniform("v", sceneObj->getTransform()->getViewMatrix());
				sh->setUniform("m", sceneObj->getTransform()->getModelMatrix());
				sh->setUniform("mvp", sceneObj->getTransform()->getMvp());
				sh->setUniform("itmv", sceneObj->getTransform()->getItmv());

				sh->setUniform("light.position", light->getPosition());
				sh->setUniform("light.color", light->getColor());
				sh->setUniform("light.intensity", light->getIntensity());
				sh->setUniform("light.direction", light->getDirection());

				phi::material* mat = sceneObj->getMaterial();
				sh->setUniform("mat.ambientColor", mat->getAmbientColor());
				sh->setUniform("mat.diffuseColor", mat->getDiffuseColor());
				sh->setUniform("mat.specularColor", mat->getSpecularColor());
				sh->setUniform("mat.ka", mat->getKa());
				sh->setUniform("mat.kd", mat->getKd());
				sh->setUniform("mat.ks", mat->getKs());
				sh->setUniform("mat.shininess", mat->getShininess());

				sh->setUniform("diffuseMap", mat->getDiffuseTexture());
				sh->setUniform("normalMap", mat->getNormalTexture());
				sh->setUniform("specularMap", mat->getSpecularTexture());

				sceneObj->render();
			}

			sh->unbind();

			glDepthFunc(GL_LESS);
			glDepthMask(true);
		}

		glDisable(GL_BLEND);
	}

	void fsSceneRenderer::fsPointLightPasses()
	{
		auto pointLights = _scene->getPointLights();
		auto pointLightsCount = pointLights->size();

		if (pointLightsCount == 0)
			return;

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		for (GLuint l = 0; l< pointLightsCount; l++)
		{
			pointLight* light = (*pointLights)[l];

			shader* sh = shaderManager::get()->getShader("FS_POINT_LIGHT");

			sh->bind();

			glDepthMask(false);
			glDepthFunc(GL_EQUAL);

			for (GLuint i = 0; i < _allObjectsCount; i++)
			{
				sceneObject* sceneObj = (*_allObjects)[i];

				sh->setUniform("p", sceneObj->getTransform()->getProjectionMatrix());
				sh->setUniform("v", sceneObj->getTransform()->getViewMatrix());
				sh->setUniform("m", sceneObj->getTransform()->getModelMatrix());
				sh->setUniform("mvp", sceneObj->getTransform()->getMvp());
				sh->setUniform("itmv", sceneObj->getTransform()->getItmv());

				sh->setUniform("light.position", light->getPosition());
				sh->setUniform("light.color", light->getColor());
				sh->setUniform("light.intensity", light->getIntensity());
				sh->setUniform("light.attenuation.constant", light->getAttenuation().constant);
				sh->setUniform("light.attenuation.linear", light->getAttenuation().linear);
				sh->setUniform("light.attenuation.exponential", light->getAttenuation().exponential);
				sh->setUniform("light.range", light->getRange());

				phi::material* mat = sceneObj->getMaterial();
				sh->setUniform("mat.diffuseColor", mat->getDiffuseColor());
				sh->setUniform("mat.specularColor", mat->getSpecularColor());
				sh->setUniform("mat.kd", mat->getKd());
				sh->setUniform("mat.ks", mat->getKs());
				sh->setUniform("mat.shininess", mat->getShininess());

				sh->setUniform("diffuseMap", mat->getDiffuseTexture());
				sh->setUniform("normalMap", mat->getNormalTexture());
				sh->setUniform("specularMap", mat->getSpecularTexture());

				sceneObj->render();
			}

			sh->unbind();

			glDepthFunc(GL_LESS);
			glDepthMask(true);
		}

		glDisable(GL_BLEND);
	}

	void fsSceneRenderer::fsSpotLightPasses()
	{
		auto spotLights = _scene->getSpotLights();
		auto spotLightsCount = spotLights->size();

		if (spotLightsCount == 0)
			return;

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		for (GLuint l = 0; l< spotLightsCount; l++)
		{
			spotLight* light = (*spotLights)[l];

			shader* sh = shaderManager::get()->getShader("FS_SPOT_LIGHT");

			sh->bind();

			glDepthMask(false);
			glDepthFunc(GL_EQUAL);

			for (GLuint i = 0; i < _allObjectsCount; i++)
			{
				sceneObject* sceneObj = (*_allObjects)[i];

				sh->setUniform("p", sceneObj->getTransform()->getProjectionMatrix());
				sh->setUniform("v", sceneObj->getTransform()->getViewMatrix());
				sh->setUniform("m", sceneObj->getTransform()->getModelMatrix());
				sh->setUniform("mvp", sceneObj->getTransform()->getMvp());
				sh->setUniform("itmv", sceneObj->getTransform()->getItmv());

				sh->setUniform("light.position", light->getPosition());
				sh->setUniform("light.color", light->getColor());
				sh->setUniform("light.intensity", light->getIntensity());
				sh->setUniform("light.attenuation.constant", light->getAttenuation().constant);
				sh->setUniform("light.attenuation.linear", light->getAttenuation().linear);
				sh->setUniform("light.attenuation.exponential", light->getAttenuation().exponential);
				sh->setUniform("light.range", light->getRange());
				sh->setUniform("light.direction", light->getDirection());
				sh->setUniform("light.cutoff", light->getCutoff());

				phi::material* mat = sceneObj->getMaterial();
				sh->setUniform("mat.diffuseColor", mat->getDiffuseColor());
				sh->setUniform("mat.specularColor", mat->getSpecularColor());
				sh->setUniform("mat.kd", mat->getKd());
				sh->setUniform("mat.ks", mat->getKs());
				sh->setUniform("mat.shininess", mat->getShininess());

				sh->setUniform("diffuseMap", mat->getDiffuseTexture());
				sh->setUniform("normalMap", mat->getNormalTexture());
				sh->setUniform("specularMap", mat->getSpecularTexture());

				sceneObj->render();
			}

			sh->unbind();

			glDepthFunc(GL_LESS);
			glDepthMask(true);
		}

		glDisable(GL_BLEND);
	}
}