#include "fsSceneRenderer.h"

namespace phi
{
    fsSceneRenderer::fsSceneRenderer(size<GLuint> viewportSize) : sceneRenderer(viewportSize)
    {
        _frameBuffer = new frameBuffer("fsSceneRenderer", viewportSize, color::transparent);
        _frameBuffer->init();

        createDefaultRenderTarget();
        createDepthBuffer();
        createSelectedObjectsRenderTarget();

        _frameBuffer->bind();
        _frameBuffer->enable(GL_CULL_FACE);
		_frameBuffer->enable(GL_DEPTH_TEST);
        _frameBuffer->unbind();

        createAmbientLightShader();
        createDirLightShader();
        createPointLightShader();
        createSpotLightShader();
    }

    fsSceneRenderer::~fsSceneRenderer()
    {
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

    void fsSceneRenderer::createAmbientLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        shader* s = shaderManager::get()->loadShader("FS_AMBIENT_LIGHT", "fs_ambient_light.vert", "fs_ambient_light.frag", attribs);

        s->addUniform("mvp");
        s->addUniform("ambientLightColor");
        s->addUniform("diffuseMap");
        s->addUniform("mat.ambientColor");
        s->addUniform("mat.ka");
        
        s->addUniform("isSelected");
        s->addUniform("id");

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
        s->addUniform("mat.kd");
        s->addUniform("mat.ks");
        s->addUniform("mat.shininess");

        s->addUniform("diffuseMap");
        s->addUniform("normalMap");
        s->addUniform("specularMap");

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
        s->addUniform("mat.kd");
        s->addUniform("mat.ks");
        s->addUniform("mat.shininess");

        s->addUniform("diffuseMap");
        s->addUniform("normalMap");
        s->addUniform("specularMap");

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

            _modelMatrices[sceneObj->getSceneId()] = modelMatrix;
            _mvpMatrices[sceneObj->getSceneId()] = _projMatrix * _viewMatrix * modelMatrix;
            _itmvMatrices[sceneObj->getSceneId()] = glm::inverse(glm::transpose(_viewMatrix * modelMatrix));
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
            
            glm::mat4 modelMatrix = _modelMatrices[sceneObj->getSceneId()];
            glm::mat4 mvp = _mvpMatrices[sceneObj->getSceneId()];
            
            sh->setUniform("mvp", mvp);
            sh->setUniform("ambientLightColor", _scene->getAmbientColor());
            sh->setUniform("id", sceneObj->getSceneId());
            sh->setUniform("isSelected", sceneObj->getSelected());

            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();
            
            for (GLuint j = 0; j < meshesCount; j++)
		    {
                mesh* m = meshes[j];

                material* mat = m->getMaterial();

                sh->setUniform("diffuseMap", mat->getDiffuseTexture());
                sh->setUniform("mat.ambientColor", mat->getAmbientColor());
                sh->setUniform("mat.ka", mat->getKa());
               
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

                glm::mat4 modelMatrix = _modelMatrices[sceneObj->getSceneId()];
                glm::mat4 mvp = _mvpMatrices[sceneObj->getSceneId()];
                glm::mat4 itmv = _itmvMatrices[sceneObj->getSceneId()];

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

                glm::mat4 modelMatrix = _modelMatrices[sceneObj->getSceneId()];
                glm::mat4 mvp = _mvpMatrices[sceneObj->getSceneId()];
                glm::mat4 itmv = _itmvMatrices[sceneObj->getSceneId()];

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
                    sh->setUniform("mat.kd", mat->getKd());
                    sh->setUniform("mat.ks", mat->getKs());
                    sh->setUniform("mat.shininess", mat->getShininess());

                    sh->setUniform("diffuseMap", mat->getDiffuseTexture());
                    sh->setUniform("normalMap", mat->getNormalTexture());
                    sh->setUniform("specularMap", mat->getSpecularTexture());

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

                glm::mat4 modelMatrix = _modelMatrices[sceneObj->getSceneId()];
                glm::mat4 mvp = _mvpMatrices[sceneObj->getSceneId()];
                glm::mat4 itmv = _itmvMatrices[sceneObj->getSceneId()];

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
                    sh->setUniform("mat.kd", mat->getKd());
                    sh->setUniform("mat.ks", mat->getKs());
                    sh->setUniform("mat.shininess", mat->getShininess());

                    sh->setUniform("diffuseMap", mat->getDiffuseTexture());
                    sh->setUniform("normalMap", mat->getNormalTexture());
                    sh->setUniform("specularMap", mat->getSpecularTexture());

                    meshRenderer::render(m);
                }
            }

            sh->unbind();

            glDepthFunc(GL_LESS);
            glDepthMask(true);
        }

        glDisable(GL_BLEND);
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
        sh->setUniform("selectionMap", selectedRenderTarget->getTexture());

        meshRenderer::render(&_quad);

        sh->unbind();

        glDisable(GL_BLEND);
    }

    void fsSceneRenderer::onRender()
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