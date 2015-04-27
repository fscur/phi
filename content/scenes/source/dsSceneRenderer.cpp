#include "dsSceneRenderer.h"
#include "quad.h"

namespace phi
{
    dsSceneRenderer::dsSceneRenderer(size<GLuint> viewportSize) : sceneRenderer(viewportSize)
    {
        initBuffers();

        createGeomPassShader();
        createStencilShader();
        createDirLightShader();
        createPointLightShader();
        createSpotLightShader();
    }

    dsSceneRenderer::~dsSceneRenderer()
    {
        DELETE(_frameBuffer);
    }

    void dsSceneRenderer::initBuffers()
    {
        _frameBuffer = new frameBuffer("dsSceneRenderer", _viewportSize, color::transparent);
        _frameBuffer->init();
        _frameBuffer->bind();

        createRT0();
        createRT1();
        createRT2();
        createRT3();
        createRT4();

        _frameBuffer->bind();
        _frameBuffer->enable(GL_CULL_FACE);
        _frameBuffer->enable(GL_DEPTH_TEST);
        _frameBuffer->unbind();
    }

    void dsSceneRenderer::createRT0()
    {
        texture* t = texture::create(_viewportSize, GL_RGBA);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        renderTarget* r = _frameBuffer->newRenderTarget(
            "rt0",
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0);

        _frameBuffer->addRenderTarget(r);
    }

    void dsSceneRenderer::createRT1()
    {
        texture* t = texture::create(_viewportSize, GL_RGBA16F);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        renderTarget* r = _frameBuffer->newRenderTarget(
            "rt1", 
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT1);

        _frameBuffer->addRenderTarget(r);
    }

    void dsSceneRenderer::createRT2()
    {
        texture* t = texture::create(_viewportSize, GL_RGBA16F);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        renderTarget* r = _frameBuffer->newRenderTarget(
            "rt2", 
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT2);

        _frameBuffer->addRenderTarget(r);
    }

    void dsSceneRenderer::createRT3()
    {
        texture* t = texture::create(_viewportSize, GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        renderTarget* d = _frameBuffer->newRenderTarget(
            "rt3", 
            t, 
            GL_DRAW_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT);

        _frameBuffer->addRenderTarget(d);
    }

    void dsSceneRenderer::createRT4()
    {
        texture* t = renderingSystem::pickingFrameBuffer->getPickingTexture();

        renderTarget* r = _frameBuffer->newRenderTarget(
            "rt4", 
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT3);

        _frameBuffer->addRenderTarget(r);
    }

    void dsSceneRenderer::createGeomPassShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");

        shader* s = shaderManager::get()->loadShader("DS_GEOM_PASS", "ds_geom_pass.vert", "ds_geom_pass.frag", attribs);

        s->addUniform("mv");
        s->addUniform("mvp");
        s->addUniform("itmv");

        s->addUniform("ambientLightColor");

        s->addUniform("mat.ambientColor");
        s->addUniform("mat.diffuseColor");
        s->addUniform("mat.specularColor");
        s->addUniform("mat.ka");
        s->addUniform("mat.kd");
        s->addUniform("mat.ks");
        s->addUniform("mat.shininess");

        s->addUniform("diffuseMap");
        s->addUniform("normalMap");
        s->addUniform("specularMap");

        s->addUniform("isSelected");
        s->addUniform("id");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRenderer::createStencilShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");

        shader* s = shaderManager::get()->loadShader("DS_STENCIL", "ds_stencil.vert", "ds_stencil.frag", attribs);

        s->addUniform("mvp");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRenderer::createDirLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        shader* s = shaderManager::get()->loadShader("DS_DIR_LIGHT", "ds_dir_light.vert", "ds_dir_light.frag", attribs);

        s->addUniform("v");
        s->addUniform("m");
        s->addUniform("ip");

        s->addUniform("light.position");
        s->addUniform("light.color");
        s->addUniform("light.intensity");
        s->addUniform("light.direction");

        s->addUniform("rt0");
        s->addUniform("rt1");
        s->addUniform("rt2");
        s->addUniform("rt3");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRenderer::createPointLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");

        shader* s = shaderManager::get()->loadShader("DS_POINT_LIGHT", "ds_point_light.vert", "ds_point_light.frag", attribs);

        s->addUniform("v");
        s->addUniform("m");
        s->addUniform("ip");
        s->addUniform("mvp");
        s->addUniform("res");

        s->addUniform("light.position");
        s->addUniform("light.color");
        s->addUniform("light.intensity");
        s->addUniform("light.range");
        s->addUniform("light.oneOverRangeSqr");

        s->addUniform("rt0");
        s->addUniform("rt1");
        s->addUniform("rt2");
        s->addUniform("rt3");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRenderer::createSpotLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");

        shader* s = shaderManager::get()->loadShader("DS_SPOT_LIGHT", "ds_spot_light.vert", "ds_spot_light.frag", attribs);

        s->addUniform("v");
        s->addUniform("m");
        s->addUniform("ip");
        s->addUniform("mvp");
        s->addUniform("res");

        s->addUniform("light.position");
        s->addUniform("light.color");
        s->addUniform("light.intensity");
        s->addUniform("light.attenuation.constant");
        s->addUniform("light.attenuation.linear");
        s->addUniform("light.attenuation.exponential");
        s->addUniform("light.range");
        s->addUniform("light.direction");
        s->addUniform("light.cutoff");

        s->addUniform("rt0");
        s->addUniform("rt1");
        s->addUniform("rt2");
        s->addUniform("rt3");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRenderer::render()
    {
        _hasSelectedObjects = false;

        geomPass();

        pointLightPass();
        spotLightPass();

        directionalLightPass();

        glDepthMask(GL_FALSE);

        renderingSystem::defaultFrameBuffer->bindForDrawing();
        _frameBuffer->bindForReading();
        _frameBuffer->blit("rt0", 0, 0, _viewportSize.width, _viewportSize.height);

        if (_hasSelectedObjects)
            selectedObjectsPass();
    }

    void dsSceneRenderer::geomPass()
    {
        _frameBuffer->bindForDrawing();

        GLenum drawBuffers[] = 
        { 
            GL_COLOR_ATTACHMENT0, 
            GL_COLOR_ATTACHMENT1, 
            GL_COLOR_ATTACHMENT2, 
            GL_COLOR_ATTACHMENT3,
        };

        glDrawBuffers(4, drawBuffers);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

            sh->setUniform("isSelected", sceneObj->getSelected());
            sh->setUniform("id", sceneObj->getId());

            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();

            for (GLuint j = 0; j < meshesCount; j++)
            {
                mesh* m = meshes[j];

                material* mat = m->getMaterial();

                sh->setUniform("mat.ambientColor", mat->getAmbientColor());
                sh->setUniform("mat.diffuseColor", mat->getDiffuseColor());
                sh->setUniform("mat.specularColor", mat->getSpecularColor());
                sh->setUniform("mat.ka", mat->getKa());
                sh->setUniform("mat.kd", mat->getKd());
                sh->setUniform("mat.ks", mat->getKs());
                sh->setUniform("mat.shininess", mat->getShininess());

                sh->setUniform("diffuseMap", mat->getDiffuseTexture(), 0);
                sh->setUniform("normalMap", mat->getNormalTexture(), 1);
                sh->setUniform("specularMap", mat->getSpecularTexture(), 2);

                meshRenderer::render(m);
            }
        }

        sh->unbind();

        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
    }

    void dsSceneRenderer::directionalLightPass()
    {
        auto directionalLights = _scene->getDirectionalLights();
        auto directionalLightsCount = directionalLights->size();

        if (directionalLightsCount == 0)
            return;

        glDrawBuffer(GL_COLOR_ATTACHMENT0);

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
        glm::mat4 inverseProjectionMatrix = glm::inverse(_camera->getPerspProjMatrix());

        texture* rt0Texture = _frameBuffer->getRenderTarget("rt0")->getTexture();
        texture* rt1Texture = _frameBuffer->getRenderTarget("rt1")->getTexture();
        texture* rt2Texture = _frameBuffer->getRenderTarget("rt2")->getTexture();
        texture* rt3Texture = _frameBuffer->getRenderTarget("rt3")->getTexture();

        for (GLuint i = 0; i < directionalLightsCount; i++)
        {
            directionalLight* light = (*directionalLights)[i];
            sh->setUniform("v", viewMatrix);
            sh->setUniform("m", modelMatrix);
            sh->setUniform("ip", inverseProjectionMatrix);
            sh->setUniform("light.position", light->getPosition());
            sh->setUniform("light.color", light->getColor());
            sh->setUniform("light.intensity", light->getIntensity());
            sh->setUniform("light.direction", light->getDirection());

            sh->setUniform("rt0", rt0Texture, 0);
            sh->setUniform("rt1", rt1Texture, 1);
            sh->setUniform("rt2", rt2Texture, 2);
            sh->setUniform("rt3", rt3Texture, 3);

            meshRenderer::render(&_quad);
        }

        sh->unbind();

        glDisable(GL_BLEND);
    }

    void dsSceneRenderer::pointLightPass()
    {	
        auto pointLights = _scene->getPointLights();
        auto pointLightsCount = pointLights->size();

        if (pointLightsCount == 0)
            return;

        glm::mat4 viewMatrix = _camera->getViewMatrix();
        glm::mat4 projectionMatrix = _camera->getPerspProjMatrix();
        glm::mat4 inverseProjectionMatrix = glm::inverse(_camera->getPerspProjMatrix());

        texture* rt0Texture = _frameBuffer->getRenderTarget("rt0")->getTexture();
        texture* rt1Texture = _frameBuffer->getRenderTarget("rt1")->getTexture();
        texture* rt2Texture = _frameBuffer->getRenderTarget("rt2")->getTexture();
        texture* rt3Texture = _frameBuffer->getRenderTarget("rt3")->getTexture();

        glm::vec2 resolution(_viewportSize.width, _viewportSize.height);

        shader* ss = shaderManager::get()->getShader("DS_STENCIL");
        shader* ps = shaderManager::get()->getShader("DS_POINT_LIGHT");

        glEnable(GL_STENCIL_TEST);

        mesh* _mesh;

        for (GLuint i = 0; i < pointLightsCount; i++)
        {
            pointLight* light = (*pointLights)[i];
            sphere* boundingVolume = light->getBoundingVolume();
            glm::mat4 modelMatrix = boundingVolume->getTransform()->getModelMatrix();
            glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
            _mesh = boundingVolume->getModel()->getMeshes()[0];
            // Disable color/depth write and enable stencil

            glDrawBuffer(GL_NONE);

            glEnable(GL_DEPTH_TEST);

            glDisable(GL_CULL_FACE);

            glClear(GL_STENCIL_BUFFER_BIT);

            glStencilFunc(GL_ALWAYS, 0, 0);

            glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
            glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

            ss->bind();
            ss->setUniform("mvp", mvp);
            meshRenderer::render(_mesh); 
            ss->unbind();

            glDrawBuffer(GL_COLOR_ATTACHMENT0);

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
            ps->setUniform("ip", inverseProjectionMatrix);
            ps->setUniform("mvp", mvp);
            ps->setUniform("res", resolution);

            ps->setUniform("light.position", light->getPosition());
            ps->setUniform("light.color", light->getColor());
            ps->setUniform("light.intensity", light->getIntensity());
            ps->setUniform("light.range", light->getRange());
            ps->setUniform("light.oneOverRangeSqr", light->getOneOverRangerSqr());
            ps->setUniform("rt0", rt0Texture, 0);
            ps->setUniform("rt1", rt1Texture, 1);
            ps->setUniform("rt2", rt2Texture, 2);
            ps->setUniform("rt3", rt3Texture, 3);
            
            meshRenderer::render(_mesh); 

            ps->unbind();

            glCullFace(GL_BACK);

            glDisable(GL_BLEND);
        }

        glDisable(GL_STENCIL_TEST);
    }

    void dsSceneRenderer::spotLightPass()
    {	
        auto spotLights = _scene->getSpotLights();
        auto spotLightsCount = spotLights->size();

        if (spotLightsCount == 0)
            return;

        glm::mat4 projectionMatrix = _camera->getPerspProjMatrix();
        glm::mat4 inverseProjectionMatrix = glm::inverse(_camera->getPerspProjMatrix());
        glm::mat4 viewMatrix = _camera->getViewMatrix();

        texture* rt0Texture = _frameBuffer->getRenderTarget("rt0")->getTexture();
        texture* rt1Texture = _frameBuffer->getRenderTarget("rt1")->getTexture();
        texture* rt2Texture = _frameBuffer->getRenderTarget("rt2")->getTexture();
        texture* rt3Texture = _frameBuffer->getRenderTarget("rt3")->getTexture();

        shader* ss = shaderManager::get()->getShader("DS_STENCIL");
        shader* sls = shaderManager::get()->getShader("DS_SPOT_LIGHT");

        glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);

        ss->bind();
        mesh* _mesh;
        for (GLuint i = 0; i < spotLightsCount; i++)
        {
            spotLight* light = (*spotLights)[i];
            cone* boundingVolume = light->getBoundingVolume();
            glm::mat4 modelMatrix = boundingVolume->getTransform()->getModelMatrix();
            glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
            _mesh = boundingVolume->getModel()->getMeshes()[0];

            glDrawBuffer(GL_NONE);

            glEnable(GL_DEPTH_TEST);

            glDisable(GL_CULL_FACE);

            glClear(GL_STENCIL_BUFFER_BIT);

            glStencilFunc(GL_ALWAYS, 0, 0);

            glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
            glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

            ss->bind();
            ss->setUniform("mvp", mvp);
            meshRenderer::render(_mesh); 
            ss->unbind();

            glDrawBuffer(GL_COLOR_ATTACHMENT0);

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
            sls->setUniform("ip", inverseProjectionMatrix);
            sls->setUniform("mvp", mvp);
            sls->setUniform("res", resolution);

            sls->setUniform("light.position", light->getPosition());
            sls->setUniform("light.color", light->getColor());
            sls->setUniform("light.intensity", light->getIntensity());
            sls->setUniform("light.range", light->getRange());
            sls->setUniform("light.oneOverRangeSqr", light->getOneOverRangerSqr());
            sls->setUniform("light.direction", light->getDirection());
            sls->setUniform("light.cutoff", light->getCutoff());

            sls->setUniform("rt0", rt0Texture, 0);
            sls->setUniform("rt1", rt1Texture, 1);
            sls->setUniform("rt2", rt2Texture, 2);
            sls->setUniform("rt3", rt3Texture, 3);

            //TODO: Cull lights
            
            meshRenderer::render(_mesh);

            sls->unbind();

            glCullFace(GL_BACK);

            glDisable(GL_BLEND);
        }

        glDisable(GL_STENCIL_TEST);
    }

    void dsSceneRenderer::selectedObjectsPass()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);

        renderTarget* selectedRenderTarget = _frameBuffer->getRenderTarget("rt4");
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

    void dsSceneRenderer::resize(size<GLuint> size)
    {
        _viewportSize = size;
        initBuffers();
    }

    void dsSceneRenderer::onRender()
    {
        render();
    }
}