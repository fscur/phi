#include "basicSceneRenderer.h"
#include "renderingSystem.h"

namespace phi
{
	basicSceneRenderer::basicSceneRenderer(size<GLuint> viewportSize) : sceneRenderer(viewportSize)
	{
		_frameBuffer = new frameBuffer("basicSceneRenderer", viewportSize, color::transparent);
		_frameBuffer->init();
        _frameBuffer->bind();

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

		renderTarget* r = _frameBuffer->newRenderTarget(
            "default", 
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0);

		_frameBuffer->addRenderTarget(r);
	}

	void basicSceneRenderer::createSelectedObjectsRenderTarget()
	{
		texture* t = renderingSystem::pickingFrameBuffer->getPickingTexture();

		renderTarget* r = _frameBuffer->newRenderTarget(
            "selected", 
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT1);

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
		s->addUniform("selectionColor");
		s->addUniform("diffuseMap");
		s->addUniform("diffuseColor");

		shaderManager::get()->addShader(s->getName(), s);
	}

    color getSelectionColor(int objectId, int meshId, bool selected)
    {
        //objectId max = 4095
        //meshId max = 1048575

        unsigned int id = objectId << 20;
        id += meshId;

	    unsigned int r = id & 255;
        id = id >> 8;
        unsigned int g = id & 255;
        id = id >> 8;
        unsigned int b = id & 255;

        return color((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, selected ? 1.0f : 0.0f);
    }

	void basicSceneRenderer::render()
	{
		shader* sh = shaderManager::get()->getShader("BASIC_GEOM_PASS");
		sh->bind();

		for (GLuint i = 0; i < _allObjectsCount; i++)
		{
			sceneObject* sceneObj = (*_allObjects)[i];
            
			sh->setUniform("mvp", sceneObj->getTransform()->getMvp());
			
            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();

            for (GLuint j = 0; j < meshesCount; j++)
		    {
                mesh* m = meshes[j];

                material* mat = m->getMaterial();

                bool selected = sceneObj->getSelected() || m->getSelected();

                sh->setUniform("selectionColor", getSelectionColor(sceneObj->getId(), m->getId(), selected));

			    sh->setUniform("diffuseMap", mat->getDiffuseTexture());
				sh->setUniform("diffuseColor", mat->getDiffuseColor());

			    meshRenderer::render(m);
            }
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