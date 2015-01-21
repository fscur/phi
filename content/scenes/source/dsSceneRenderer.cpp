#include "dsSceneRenderer.h"
#include "quad.h"

namespace phi
{
	dsSceneRenderer::dsSceneRenderer(size<GLuint> viewportSize) : sceneRenderer(viewportSize)
	{
		_gBuffer = new gBuffer(viewportSize);
		_gBuffer->init();
	}

	dsSceneRenderer::~dsSceneRenderer()
	{
		DELETE(_gBuffer);
	}

	void dsSceneRenderer::onRender()
	{
		_hasSelectedObjects = false;

		dsGeomPass();
		//dsAmbientLightBlit();
		//dsPointLightPass();
		//dsSpotLightPass();
		//dsDirectionalLightPass();
		
		//if (_hasSelectedObjects)
			//dsSelectedObjectsPass();
		
		dsFinalBlit();
	}

	void dsSceneRenderer::dsAmbientLightBlit()
	{
		_gBuffer->bindForLightPass();
		_gBuffer->bindForReading();
		_gBuffer->blit(1, 0, 0, _viewportSize.width, _viewportSize.height);
	}

	void dsSceneRenderer::dsGeomPass()
	{
		_gBuffer->bindForGeomPass();

		glDepthMask(GL_TRUE);
		_gBuffer->clear();

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		shader* sh = shaderManager::get()->getShader("DS_GEOM_PASS");
		sh->bind();

		for (GLuint i = 0; i < _allObjectsCount; i++)
		{
			sceneObject* sceneObj = (*_allObjects)[i];

			if(!_hasSelectedObjects)
				_hasSelectedObjects = sceneObj->getSelected();

			sh->setUniform("mv", sceneObj->getTransform()->getMv());
			sh->setUniform("mvp", sceneObj->getTransform()->getMvp());
			sh->setUniform("itmv", sceneObj->getTransform()->getItmv());

			sh->setUniform("ambientLightColor", _scene->getAmbientColor());

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

			sh->setUniform("isSelected", sceneObj->getSelected());

			sceneObj->render();
		}

		sh->unbind();

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
	}

	void dsSceneRenderer::dsDirectionalLightPass()
	{
		auto directionalLights = _scene->getDirectionalLights();
		auto directionalLightsCount = directionalLights->size();

		if (directionalLightsCount == 0)
			return;

		_gBuffer->bindForLightPass();

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		shader* sh = shaderManager::get()->getShader("DS_DIR_LIGHT");

		sh->bind();

		glm::mat4 modelMatrix = glm::mat4(
			2.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		glm::mat4 viewMatrix = _camera->getViewMatrix();

		texture* positionMap = _gBuffer->getPositionTexture();
		texture* normalMap = _gBuffer->getNormalTexture();
		texture* diffuseMap = _gBuffer->getDiffuseTexture();
		texture* specularMap = _gBuffer->getSpecularTexture();
		texture* shininessMap = _gBuffer->getShininessTexture();

		for (GLuint i = 0; i < directionalLightsCount; i++)
		{
			directionalLight* light = (*directionalLights)[i];

			sh->setUniform("v", viewMatrix);
			sh->setUniform("m", modelMatrix);

			sh->setUniform("light.position", light->getPosition());
			sh->setUniform("light.color", light->getColor());
			sh->setUniform("light.intensity", light->getIntensity());
			sh->setUniform("light.direction", light->getDirection());

			sh->setUniform("positionMap", positionMap);
			sh->setUniform("normalMap", normalMap);
			sh->setUniform("diffuseMap", diffuseMap);
			sh->setUniform("specularMap", specularMap);
			sh->setUniform("shininessMap", shininessMap);

			meshRenderer::render(&_quad);
		}

		sh->unbind();

		glDisable(GL_BLEND);
	}

	void dsSceneRenderer::dsPointLightPass()
	{	
		auto pointLights = _scene->getPointLights();
		auto pointLightsCount = pointLights->size();

		if (pointLightsCount == 0)
			return;

		glm::mat4 projectionMatrix = _camera->getPerspProjMatrix();
		glm::mat4 viewMatrix = _camera->getViewMatrix();
		texture* positionMap = _gBuffer->getPositionTexture();
		texture* normalMap = _gBuffer->getNormalTexture();
		texture* diffuseMap = _gBuffer->getDiffuseTexture();
		texture* specularMap = _gBuffer->getSpecularTexture();
		texture* shininessMap = _gBuffer->getShininessTexture();

		glm::vec2 resolution(_viewportSize.width, _viewportSize.height);

		shader* ss = shaderManager::get()->getShader("DS_STENCIL");
		shader* ps = shaderManager::get()->getShader("DS_POINT_LIGHT");

		glEnable(GL_STENCIL_TEST);

		for (GLuint i = 0; i < pointLightsCount; i++)
		{
			pointLight* light = (*pointLights)[i];
			sphere* boundingVolume = light->getBoundingVolume();
			glm::mat4 modelMatrix = boundingVolume->getTransform()->getModelMatrix();
			glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

			// Disable color/depth write and enable stencil
			_gBuffer->bindForStencilPass();

			glEnable(GL_DEPTH_TEST);

			glDisable(GL_CULL_FACE);

			glClear(GL_STENCIL_BUFFER_BIT);

			// We need the stencil test to be enabled but we want it
			// to succeed always. Only the depth test matters.
			glStencilFunc(GL_ALWAYS, 0, 0);

			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

			ss->bind();
			ss->setUniform("mvp", mvp);
			boundingVolume->render(); 
			ss->unbind();

			_gBuffer->bindForLightPass();

			glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			ps->bind();

			ps->setUniform("v", viewMatrix);
			ps->setUniform("m", modelMatrix);
			ps->setUniform("mvp", mvp);
			ps->setUniform("res", resolution);

			ps->setUniform("light.position", light->getPosition());
			ps->setUniform("light.color", light->getColor());
			ps->setUniform("light.intensity", light->getIntensity());
			ps->setUniform("light.attenuation.constant", light->getAttenuation().constant);
			ps->setUniform("light.attenuation.linear", light->getAttenuation().linear);
			ps->setUniform("light.attenuation.exponential", light->getAttenuation().exponential);
			ps->setUniform("light.range", light->getRange());

			ps->setUniform("positionMap", positionMap);
			ps->setUniform("normalMap", normalMap);
			ps->setUniform("diffuseMap", diffuseMap);
			ps->setUniform("specularMap", specularMap);
			ps->setUniform("shininessMap", shininessMap);

			boundingVolume->render();

			ps->unbind();

			glCullFace(GL_BACK);

			glDisable(GL_BLEND);
		}

		glDisable(GL_STENCIL_TEST);
	}

	void dsSceneRenderer::dsSpotLightPass()
	{	
		auto spotLights = _scene->getSpotLights();
		auto spotLightsCount = spotLights->size();

		if (spotLightsCount == 0)
			return;

		glm::mat4 projectionMatrix = _camera->getPerspProjMatrix();
		glm::mat4 viewMatrix = _camera->getViewMatrix();
		texture* positionMap = _gBuffer->getPositionTexture();
		texture* normalMap = _gBuffer->getNormalTexture();
		texture* diffuseMap = _gBuffer->getDiffuseTexture();
		texture* specularMap = _gBuffer->getSpecularTexture();
		texture* shininessMap = _gBuffer->getShininessTexture();

		shader* ss = shaderManager::get()->getShader("DS_STENCIL");
		shader* sls = shaderManager::get()->getShader("DS_SPOT_LIGHT");

		glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);

		ss->bind();

		for (GLuint i = 0; i < spotLightsCount; i++)
		{
			spotLight* light = (*spotLights)[i];
			cone* boundingVolume = light->getBoundingVolume();
			glm::mat4 modelMatrix = boundingVolume->getTransform()->getModelMatrix();
			glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

			_gBuffer->bindForStencilPass();

			glEnable(GL_DEPTH_TEST);

			glDisable(GL_CULL_FACE);

			glClear(GL_STENCIL_BUFFER_BIT);

			// We need the stencil test to be enabled but we want it
			// to succeed always. Only the depth test matters.
			glStencilFunc(GL_ALWAYS, 0, 0);

			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

			ss->bind();
			ss->setUniform("mvp", mvp);
			boundingVolume->render(); 
			ss->unbind();

			_gBuffer->bindForLightPass();

			glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			sls->bind();

			sls->setUniform("v", viewMatrix);
			sls->setUniform("m", modelMatrix);
			sls->setUniform("mvp", mvp);
			sls->setUniform("res", resolution);

			sls->setUniform("light.position", light->getPosition());
			sls->setUniform("light.color", light->getColor());
			sls->setUniform("light.intensity", light->getIntensity());
			sls->setUniform("light.attenuation.constant", light->getAttenuation().constant);
			sls->setUniform("light.attenuation.linear", light->getAttenuation().linear);
			sls->setUniform("light.attenuation.exponential", light->getAttenuation().exponential);
			sls->setUniform("light.range", light->getRange());
			sls->setUniform("light.direction", light->getDirection());
			sls->setUniform("light.cutoff", light->getCutoff());

			sls->setUniform("positionMap", positionMap);
			sls->setUniform("normalMap", normalMap);
			sls->setUniform("diffuseMap", diffuseMap);
			sls->setUniform("specularMap", specularMap);
			sls->setUniform("shininessMap", shininessMap);

			//TODO: Cull lights

			boundingVolume->render(); 

			sls->unbind();

			glCullFace(GL_BACK);

			glDisable(GL_BLEND);
		}

		glDisable(GL_STENCIL_TEST);
	}

	void dsSceneRenderer::dsSelectedObjectsPass()
	{
		_gBuffer->bindForLightPass();

		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);

		shader* sh = shaderManager::get()->getShader("DS_SELECTED_OBJECTS");

		sh->bind();

		glm::mat4 modelMatrix = glm::mat4(
			2.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		texture* shininessMap = _gBuffer->getShininessTexture();

		sh->setUniform("m", modelMatrix);
		sh->setUniform("isSelectedMap", shininessMap);
		sh->setUniform("res", resolution);
		meshRenderer::render(&_quad);

		sh->unbind();

		glDisable(GL_BLEND);
	}

	void dsSceneRenderer::dsFinalBlit()
	{
		renderingSystem::defaultFrameBuffer->bindForDrawing();
		_gBuffer->bindForReading();
		_gBuffer->blit(1, 0, 0, _viewportSize.width, _viewportSize.height);
	}
}