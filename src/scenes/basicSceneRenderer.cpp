#include "phi/scenes/basicSceneRenderer.h"
#include "phi/rendering/renderingSystem.h"

namespace phi
{
    basicSceneRenderer::basicSceneRenderer(size<GLuint> viewportSize) : sceneRenderer(viewportSize)
    {
        initBuffers();
        createGeomPassShader();
    }

    basicSceneRenderer::~basicSceneRenderer()
    {
    }

    void basicSceneRenderer::initBuffers()
    {
        _frameBuffer = new frameBuffer("basicSceneRenderer", _viewportSize, color::transparent);
        _frameBuffer->init();
        _frameBuffer->bind();

        createDefaultRenderTarget();
        createSelectedObjectsRenderTarget();
        createDepthBuffer();

        _frameBuffer->bind();
        _frameBuffer->enable(GL_CULL_FACE);
        _frameBuffer->enable(GL_DEPTH_TEST);
        _frameBuffer->unbind();
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

    void basicSceneRenderer::render()
    {
        shader* sh = shaderManager::get()->getShader("BASIC_GEOM_PASS");
        sh->bind();

        glm::mat4 projectionMatrix = _camera->getPerspProjMatrix();
        glm::mat4 viewMatrix = _camera->getViewMatrix();

        for (GLuint i = 0; i < _allObjectsCount; i++)
        {
            sceneObject* sceneObj = (*_allObjects)[i];

            glm::mat4 mvp = projectionMatrix * viewMatrix * sceneObj->getModelMatrix();
            sh->setUniform("mvp", mvp);

            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();

            for (GLuint j = 0; j < meshesCount; j++)
            {
                mesh* m = meshes[j];
                sceneMesh* sm = sceneObj->getSceneMeshes()[j];
                material* mat = sm->getMaterial();

                bool selected = sm->getIsSelected();

                sh->setUniform("selectionColor", getSelectionColor(sceneObj->getId(), m->getId(), selected));
                sh->setUniform("diffuseMap", mat->getDiffuseTexture(), 0);
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
        sh->setUniform("selectionMap", selectedRenderTarget->getTexture(), 0);

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
        glViewport(0, 0, _viewportSize.width, _viewportSize.height);

        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        glClearBufferfv(GL_COLOR, 1, &color.r); // Clears the selection render target

        render();

        glDepthMask(GL_FALSE);

        renderingSystem::defaultFrameBuffer->bindForDrawing();
        _frameBuffer->bindForReading();
        _frameBuffer->blit("default", 0, 0, _viewportSize.width, _viewportSize.height);

        selectedObjectsPass();
    }
}