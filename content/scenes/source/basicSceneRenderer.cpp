#include "basicSceneRenderer.h"
#include "renderingSystem.h"

namespace phi
{
	basicSceneRenderer::basicSceneRenderer(size<GLuint> viewportSize) : sceneRenderer(viewportSize),
		_defaultDiffuseMap(renderingSystem::repository->getResource<texture>("default_diffuseMap"))
	{
		_frameBuffer = new frameBuffer("basicSceneRenderer", viewportSize, color::transparent);
		_frameBuffer->init();

		createDefaultRenderTarget();
		createSelectedObjectsRenderTarget();
		createDepthBuffer();

		_frameBuffer->bind();
		_frameBuffer->enable(GL_CULL_FACE);
		_frameBuffer->enable(GL_DEPTH_TEST);
		_frameBuffer->unbind();

		createGeomPassShader();
	}
	
	basicSceneRenderer::~basicSceneRenderer()
	{
	}

	void basicSceneRenderer::createDefaultRenderTarget()
	{
		texture* t = texture::create(_viewportSize, GL_RGB);
		t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		renderTarget* r = _frameBuffer->newRenderTarget("default", t);

		_frameBuffer->addRenderTarget(r);
	}

	void basicSceneRenderer::createSelectedObjectsRenderTarget()
	{
		texture* t = renderingSystem::pickingFrameBuffer->getPickingTexture();

		renderTarget* r = _frameBuffer->newRenderTarget("selected", t);

		_frameBuffer->addRenderTarget(r);
	}

	void basicSceneRenderer::createDepthBuffer()
	{
		texture* t = texture::create(_viewportSize, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
		t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		renderTarget* d = _frameBuffer->newRenderTarget(
			"depth", 
			t, 
			GL_DRAW_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT);

		_frameBuffer->addRenderTarget(d);
	}

	void basicSceneRenderer::createGeomPassShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");
		
		shader* s = shaderManager::get()->loadShader("BASIC_GEOM_PASS", "basic_geom_pass.vert", "basic_geom_pass.frag", attribs);

		s->addUniform("mvp");
		s->addUniform("id");
		s->addUniform("isSelected");
		s->addUniform("diffuseMap");
		s->addUniform("diffuseColor");

		shaderManager::get()->addShader(s->getName(), s);
	}

	void basicSceneRenderer::render()
	{
		shader* sh = shaderManager::get()->getShader("BASIC_GEOM_PASS");
		sh->bind();

		for (GLuint i = 0; i < _allObjectsCount; i++)
		{
			sceneObject* sceneObj = (*_allObjects)[i];

			sh->setUniform("mvp", sceneObj->getTransform()->getMvp());
			sh->setUniform("id", sceneObj->getSceneId());
			sh->setUniform("isSelected", sceneObj->getSelected());

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

	void basicSceneRenderer::selectedObjectsPass()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);

		renderTarget* selectedRenderTarget = _frameBuffer->getRenderTarget("selected");
		shader* sh = shaderManager::get()->getShader("POST_SELECTED_OBJECTS");

		sh->bind();

		glm::mat4 modelMatrix = glm::mat4(
			2.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		sh->setUniform("m", modelMatrix);
		sh->setUniform("res", resolution);
		sh->setUniform("selectionMap", selectedRenderTarget->getTexture());

		meshRenderer::render(&_quad);

		sh->unbind();

		glDisable(GL_BLEND);
	}

	void basicSceneRenderer::onRender()
	{
		_frameBuffer->bindForDrawing();

		GLenum drawBuffers[] = 
		{ 
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1
		};

		glDrawBuffers(2, drawBuffers);

		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render();
		
		glDepthMask(GL_FALSE);
		
		renderingSystem::defaultFrameBuffer->bindForDrawing();
		_frameBuffer->bindForReading();
		_frameBuffer->blit("default", 0, 0, _viewportSize.width, _viewportSize.height);

		selectedObjectsPass();
	}
}