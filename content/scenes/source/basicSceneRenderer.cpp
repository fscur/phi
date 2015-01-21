#include "basicSceneRenderer.h"
#include "renderingSystem.h"

namespace phi
{
	basicSceneRenderer::basicSceneRenderer(size<GLuint> viewportSize) : sceneRenderer(viewportSize),
		_defaultDiffuseMap(renderingSystem::repository->getResource<texture>("default_diffuseMap"))
	{
		_frameBuffer = new frameBuffer("basicSceneRenderer", viewportSize, color::black);
		_frameBuffer->init();

		createDefaultRenderTarget();
		createSelectedObjectsRenderTarget();
		createDepthBuffer();

		_frameBuffer->bind();
		_frameBuffer->setViewport(0, 0, viewportSize);
		_frameBuffer->enable(GL_CULL_FACE);
		_frameBuffer->enable(GL_DEPTH_TEST);
		_frameBuffer->unbind();

		createGeomPassShader();
		createSelectedObjetcsPassShader();
	}
	
	basicSceneRenderer::~basicSceneRenderer()
	{
	}

	void basicSceneRenderer::createDefaultRenderTarget()
	{
		texture* t = texture::create(_viewportSize);
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

	void basicSceneRenderer::createSelectedObjetcsPassShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");
		
		shader* s = shaderManager::get()->loadShader("BASIC_SELECTED_OBJECTS", "basic_selected_objects.vert", "basic_selected_objects.frag", attribs);

		s->addUniform("m");
		s->addUniform("res");
		s->addUniform("selectionMap");
		
		shaderManager::get()->addShader(s->getName(), s);
	}

	void basicSceneRenderer::render()
	{
		_frameBuffer->bindForDrawing();
		glClear(GL_COLOR_BUFFER_BIT);

		GLenum drawBuffers[] = 
		{ 
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1
		};

		glDrawBuffers(2, drawBuffers);

		glDepthMask(GL_TRUE);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		//glClear(GL_COLOR_BUFFER_BIT);

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
		
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
	}

	void basicSceneRenderer::selectedObjectsPass()
	{
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);

		renderTarget* selectedRenderTarget = _frameBuffer->getRenderTarget("selected");
		shader* sh = shaderManager::get()->getShader("BASIC_SELECTED_OBJECTS");

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
		_frameBuffer->bind();

		render();

		renderingSystem::defaultFrameBuffer->bindForDrawing();
		_frameBuffer->bindForReading();
		_frameBuffer->blit(0, 0, _viewportSize.width, _viewportSize.height);

		selectedObjectsPass();
	}
}