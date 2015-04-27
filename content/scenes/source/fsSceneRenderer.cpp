#include "fsSceneRenderer.h"

namespace phi
{
    fsSceneRenderer::fsSceneRenderer(size<GLuint> viewportSize) : sceneRenderer(viewportSize)
    {
        initBuffers();

        createAmbientLightShader();
        createDirLightShader();
        createPointLightShader();
        createSpotLightShader();

        createEmissiveBloomShaders();
    }

    fsSceneRenderer::~fsSceneRenderer()
    {
    }

    void fsSceneRenderer::initBuffers()
    {
        _frameBuffer = new frameBuffer("fsSceneRenderer", _viewportSize, color::transparent);
        _frameBuffer->init();
        _frameBuffer->bind();

        createDefaultRenderTarget();
        createDepthBuffer();
        createSelectedObjectsRenderTarget();

        _frameBuffer->bind();
        _frameBuffer->enable(GL_CULL_FACE);
        _frameBuffer->enable(GL_DEPTH_TEST);
        _frameBuffer->unbind();

        _postFrameBuffer = new frameBuffer("fsPostSceneRenderer", _viewportSize, color::transparent);
        _postFrameBuffer->init();
        _postFrameBuffer->bind();

        createPostRenderTarget();
    }

    void fsSceneRenderer::createDefaultRenderTarget()
    {
        texture* t = texture::create(_viewportSize, GL_RGBA);

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

    void fsSceneRenderer::createSelectedObjectsRenderTarget()
    {
        texture* t = renderingSystem::pickingFrameBuffer->getPickingTexture();

        renderTarget* r = _frameBuffer->newRenderTarget(
            "selected", 
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT1);

        _frameBuffer->addRenderTarget(r);
    }

    void fsSceneRenderer::createDepthBuffer()
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

    void fsSceneRenderer::createPostRenderTarget()
    {
        //GLint internalFormat = GL_RGB32F, GLint format = GL_RGBA, GLint type = GL_FLOAT, GLuint level = 0, GLvoid* data = 0
        texture* t = texture::create(_viewportSize, GL_RGBA);

        //t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        renderTarget* r = _postFrameBuffer->newRenderTarget(
            "post",
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0);

        _postFrameBuffer->addRenderTarget(r);
    }

    void fsSceneRenderer::createAmbientLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        shader* s = shaderManager::get()->loadShader("FS_AMBIENT_LIGHT", "fs_ambient_light.vert", "fs_ambient_light.frag", attribs);

        s->addUniform("mvp");
        s->addUniform("ambientLightColor");
        s->addUniform("diffuseMap");
        s->addUniform("emissiveMap");
        s->addUniform("mat.ambientColor");
        s->addUniform("mat.emissiveColor");
        s->addUniform("mat.ka");
        s->addUniform("mat.isEmissive");

        s->addUniform("selectionColor");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void fsSceneRenderer::createDirLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");

        shader* s = shaderManager::get()->loadShader("FS_DIR_LIGHT", "fs_dir_light.vert", "fs_dir_light.frag", attribs);

        s->addUniform("p");
        s->addUniform("v");
        s->addUniform("m");
        s->addUniform("mvp");
        s->addUniform("itmv");

        s->addUniform("light.position");
        s->addUniform("light.color");
        s->addUniform("light.intensity");
        s->addUniform("light.direction");

        s->addUniform("mat.diffuseColor");
        s->addUniform("mat.specularColor");
        s->addUniform("mat.emissiveColor");
        s->addUniform("mat.kd");
        s->addUniform("mat.ks");
        s->addUniform("mat.shininess");
        s->addUniform("mat.isEmissive");

        s->addUniform("diffuseMap");
        s->addUniform("normalMap");
        s->addUniform("specularMap");
        s->addUniform("emissiveMap");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void fsSceneRenderer::createPointLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");

        shader* s = shaderManager::get()->loadShader("FS_POINT_LIGHT", "fs_point_light.vert", "fs_point_light.frag", attribs);

        s->addUniform("p");
        s->addUniform("v");
        s->addUniform("m");
        s->addUniform("mvp");
        s->addUniform("itmv");

        s->addUniform("light.position");
        s->addUniform("light.color");
        s->addUniform("light.intensity");
        s->addUniform("light.range");
        s->addUniform("light.oneOverRangeSqr");

        s->addUniform("mat.diffuseColor");
        s->addUniform("mat.specularColor");
        s->addUniform("mat.emissiveColor");
        s->addUniform("mat.kd");
        s->addUniform("mat.ks");
        s->addUniform("mat.shininess");
        s->addUniform("mat.isEmissive");

        s->addUniform("diffuseMap");
        s->addUniform("normalMap");
        s->addUniform("specularMap");
        s->addUniform("emissiveMap");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void fsSceneRenderer::createSpotLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");

        shader* s = shaderManager::get()->loadShader("FS_SPOT_LIGHT", "fs_spot_light.vert", "fs_spot_light.frag", attribs);

        s->addUniform("p");
        s->addUniform("v");
        s->addUniform("m");
        s->addUniform("mvp");
        s->addUniform("itmv");

        s->addUniform("light.position");
        s->addUniform("light.color");
        s->addUniform("light.intensity");
        s->addUniform("light.range");
        s->addUniform("light.oneOverRangeSqr");
        s->addUniform("light.direction");
        s->addUniform("light.cutoff");

        s->addUniform("mat.diffuseColor");
        s->addUniform("mat.specularColor");
        s->addUniform("mat.emissiveColor");
        s->addUniform("mat.kd");
        s->addUniform("mat.ks");
        s->addUniform("mat.shininess");
        s->addUniform("mat.isEmissive");

        s->addUniform("diffuseMap");
        s->addUniform("normalMap");
        s->addUniform("specularMap");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void fsSceneRenderer::createEmissiveBloomShaders()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        shader* s = shaderManager::get()->loadShader("POST_EMISSIVE_BLOOM", "post_emissive_bloom.vert", "post_emissive_bloom.frag", attribs);

        s->addUniform("m");
        s->addUniform("res");
        s->addUniform("tex1");

        shaderManager::get()->addShader(s->getName(), s);

        s = shaderManager::get()->loadShader("POST_GAUSSIAN_BLUR", "post_gaussian_blur.vert", "post_gaussian_blur.frag", attribs);

        s->addUniform("m");
        s->addUniform("res");
        s->addUniform("tex1");
        s->addUniform("tex2");

        shaderManager::get()->addShader(s->getName(), s);

        s = shaderManager::get()->loadShader("POST_COMBINE", "post_combine.vert", "post_combine.frag", attribs);

        s->addUniform("m");
        s->addUniform("res");
        s->addUniform("tex1");
        s->addUniform("tex2");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void fsSceneRenderer::render()
    {
        _viewMatrix = _scene->getActiveCamera()->getViewMatrix();
        _projMatrix = _scene->getActiveCamera()->getPerspProjMatrix();

        for (GLuint i = 0; i < _allObjectsCount; i++)
        {
            sceneObject* sceneObj = (*_allObjects)[i];

            glm::mat4 modelMatrix = sceneObj->getTransform()->getModelMatrix();

            _modelMatrices[sceneObj->getId()] = modelMatrix;
            _mvpMatrices[sceneObj->getId()] = _projMatrix * _viewMatrix * modelMatrix;
            _itmvMatrices[sceneObj->getId()] = glm::inverse(glm::transpose(_viewMatrix * modelMatrix));
        }

        ambientLightPass();
        dirLightPasses();
        pointLightPasses();
        spotLightPasses();
    }

    void fsSceneRenderer::ambientLightPass()
    {
        shader* sh = shaderManager::get()->getShader("FS_AMBIENT_LIGHT");
        sh->bind();

        for (GLuint i = 0; i < _allObjectsCount; i++)
        {
            sceneObject* sceneObj = (*_allObjects)[i];

            if(!_hasSelectedObjects)
                _hasSelectedObjects = sceneObj->getSelected();

            glm::mat4 modelMatrix = _modelMatrices[sceneObj->getId()];
            glm::mat4 mvp = _mvpMatrices[sceneObj->getId()];

            sh->setUniform("mvp", mvp);
            sh->setUniform("ambientLightColor", _scene->getAmbientColor());

            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();

            for (GLuint j = 0; j < meshesCount; j++)
            {
                mesh* m = meshes[j];

                material* mat = m->getMaterial();

                bool selected = sceneObj->getSelected() || m->getSelected();

                sh->setUniform("selectionColor", getSelectionColor(sceneObj->getId(), m->getId(), selected));

                sh->setUniform("diffuseMap", mat->getDiffuseTexture(), 0);
                sh->setUniform("emissiveMap", mat->getEmissiveTexture(), 1);

                sh->setUniform("mat.ambientColor", mat->getAmbientColor());
                sh->setUniform("mat.emissiveColor", mat->getEmissiveColor());
                sh->setUniform("mat.ka", mat->getKa());
                sh->setUniform("mat.isEmissive", mat->getIsEmissive());

                meshRenderer::render(m);
            }
        }

        sh->unbind();
    }

    void fsSceneRenderer::dirLightPasses()
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

                glm::mat4 modelMatrix = _modelMatrices[sceneObj->getId()];
                glm::mat4 mvp = _mvpMatrices[sceneObj->getId()];
                glm::mat4 itmv = _itmvMatrices[sceneObj->getId()];

                sh->setUniform("p", _projMatrix);
                sh->setUniform("v", _viewMatrix);
                sh->setUniform("m", modelMatrix);
                sh->setUniform("mvp", mvp);
                sh->setUniform("itmv", itmv);

                sh->setUniform("light.position", light->getPosition());
                sh->setUniform("light.color", light->getColor());
                sh->setUniform("light.intensity", light->getIntensity());
                sh->setUniform("light.direction", light->getDirection());

                std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
                auto meshesCount = meshes.size();

                for (GLuint j = 0; j < meshesCount; j++)
                {
                    mesh* m = meshes[j];
                    material* mat = m->getMaterial();

                    sh->setUniform("mat.diffuseColor", mat->getDiffuseColor());
                    sh->setUniform("mat.specularColor", mat->getSpecularColor());
                    sh->setUniform("mat.emissiveColor", mat->getEmissiveColor());
                    sh->setUniform("mat.kd", mat->getKd());
                    sh->setUniform("mat.ks", mat->getKs());
                    sh->setUniform("mat.shininess", mat->getShininess());
                    sh->setUniform("mat.isEmissive", mat->getIsEmissive());

                    sh->setUniform("diffuseMap", mat->getDiffuseTexture(), 0);
                    sh->setUniform("normalMap", mat->getNormalTexture(), 1);
                    sh->setUniform("specularMap", mat->getSpecularTexture(), 2);
                    sh->setUniform("emissiveMap", mat->getEmissiveTexture(), 3);

                    meshRenderer::render(m);
                }
            }

            sh->unbind();

            glDepthFunc(GL_LESS);
            glDepthMask(true);
        }

        glDisable(GL_BLEND);
    }

    void fsSceneRenderer::pointLightPasses()
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

                glm::mat4 modelMatrix = _modelMatrices[sceneObj->getId()];
                glm::mat4 mvp = _mvpMatrices[sceneObj->getId()];
                glm::mat4 itmv = _itmvMatrices[sceneObj->getId()];

                sh->setUniform("p", _projMatrix);
                sh->setUniform("v", _viewMatrix);
                sh->setUniform("m", modelMatrix);
                sh->setUniform("mvp", mvp);
                sh->setUniform("itmv", itmv);

                sh->setUniform("light.position", light->getPosition());
                sh->setUniform("light.color", light->getColor());
                sh->setUniform("light.intensity", light->getIntensity());
                sh->setUniform("light.range", light->getRange());
                sh->setUniform("light.oneOverRangeSqr", light->getOneOverRangerSqr());

                std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
                auto meshesCount = meshes.size();

                for (GLuint j = 0; j < meshesCount; j++)
                {
                    mesh* m = meshes[j];
                    material* mat = m->getMaterial();

                    sh->setUniform("mat.diffuseColor", mat->getDiffuseColor());
                    sh->setUniform("mat.specularColor", mat->getSpecularColor());
                    sh->setUniform("mat.emissiveColor", mat->getEmissiveColor());
                    sh->setUniform("mat.kd", mat->getKd());
                    sh->setUniform("mat.ks", mat->getKs());
                    sh->setUniform("mat.shininess", mat->getShininess());
                    sh->setUniform("mat.isEmissive", mat->getIsEmissive());

                    sh->setUniform("diffuseMap", mat->getDiffuseTexture(), 0);
                    sh->setUniform("normalMap", mat->getNormalTexture(), 1);
                    sh->setUniform("specularMap", mat->getSpecularTexture(), 2);
                    sh->setUniform("emissiveMap", mat->getEmissiveTexture(), 3);

                    meshRenderer::render(m);
                }
            }

            sh->unbind();

            glDepthFunc(GL_LESS);
            glDepthMask(true);
        }

        glDisable(GL_BLEND);
    }

    void fsSceneRenderer::spotLightPasses()
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

                glm::mat4 modelMatrix = _modelMatrices[sceneObj->getId()];
                glm::mat4 mvp = _mvpMatrices[sceneObj->getId()];
                glm::mat4 itmv = _itmvMatrices[sceneObj->getId()];

                sh->setUniform("p", _projMatrix);
                sh->setUniform("v", _viewMatrix);
                sh->setUniform("m", modelMatrix);
                sh->setUniform("mvp", mvp);
                sh->setUniform("itmv", itmv);

                sh->setUniform("light.position", light->getPosition());
                sh->setUniform("light.color", light->getColor());
                sh->setUniform("light.intensity", light->getIntensity());
                sh->setUniform("light.range", light->getRange());
                sh->setUniform("light.oneOverRangeSqr", light->getOneOverRangerSqr());
                sh->setUniform("light.direction", light->getDirection());
                sh->setUniform("light.cutoff", light->getCutoff());

                std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
                auto meshesCount = meshes.size();

                for (GLuint j = 0; j < meshesCount; j++)
                {
                    mesh* m = meshes[j];
                    material* mat = m->getMaterial();

                    sh->setUniform("mat.diffuseColor", mat->getDiffuseColor());
                    sh->setUniform("mat.specularColor", mat->getSpecularColor());
                    sh->setUniform("mat.emissiveColor", mat->getEmissiveColor());
                    sh->setUniform("mat.kd", mat->getKd());
                    sh->setUniform("mat.ks", mat->getKs());
                    sh->setUniform("mat.shininess", mat->getShininess());
                    sh->setUniform("mat.isEmissive", mat->getIsEmissive());

                    sh->setUniform("diffuseMap", mat->getDiffuseTexture(), 0);
                    sh->setUniform("normalMap", mat->getNormalTexture(), 1);
                    sh->setUniform("specularMap", mat->getSpecularTexture(), 2);

                    meshRenderer::render(m);
                }
            }

            sh->unbind();

            glDepthFunc(GL_LESS);
            glDepthMask(true);
        }

        glDisable(GL_BLEND);
    }

    void fsSceneRenderer::emissiveBloomPass()
    {
        //intensity filter

        _postFrameBuffer->bindForDrawing();

        //_frameBuffer->bindForDrawing();

        glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);

        renderTarget* curRenderTarget = _frameBuffer->getRenderTarget("default");
        shader* sh = shaderManager::get()->getShader("POST_EMISSIVE_BLOOM");

        sh->bind();

        glm::mat4 modelMatrix = glm::mat4(
            2.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        sh->setUniform("m", modelMatrix);
        sh->setUniform("res", resolution);
        sh->setUniform("tex1", curRenderTarget->getTexture(), 0);

        meshRenderer::render(&_quad);

        sh->unbind();

        //gaussian-blur filter
        _frameBuffer->bindForDrawing();
        texture* tex1 = _postFrameBuffer->getRenderTarget("post")->getTexture();
        texture* tex2 = _frameBuffer->getRenderTarget("default")->getTexture();

        sh = shaderManager::get()->getShader("POST_GAUSSIAN_BLUR");

        sh->bind();

        sh->setUniform("m", modelMatrix);
        sh->setUniform("res", resolution);

        sh->setUniform("tex1", tex1, 0);
        sh->setUniform("tex2", tex2, 1);

        meshRenderer::render(&_quad);

        sh->unbind();
    }

    void fsSceneRenderer::selectedObjectsPass()
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

    void fsSceneRenderer::resize(size<GLuint> size)
    {
        _viewportSize = size;
        initBuffers();
    }

    void fsSceneRenderer::onRender()
    {
        _hasSelectedObjects = false;

        _frameBuffer->bindForDrawing();

        GLenum drawBuffers[] = 
        {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1
        };

        glDrawBuffers(2, drawBuffers);

        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        glClearBufferfv(GL_COLOR, 1, &color.r); // Clears the selection render target

        render();

        glDepthMask(GL_FALSE);

        //emissiveBloomPass();

        renderingSystem::defaultFrameBuffer->bindForDrawing();
        _frameBuffer->bindForReading();
        _frameBuffer->blit("default", 0, 0, _viewportSize.width, _viewportSize.height);

        if (_hasSelectedObjects)
            selectedObjectsPass();
    }
}