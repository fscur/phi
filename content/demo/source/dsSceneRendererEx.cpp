#include "dsSceneRendererEx.h"
#include "quad.h"
#include "application.h"
#include "path.h"

namespace phi
{
    int pointLightIndex = 0;

    dsSceneRendererEx::dsSceneRendererEx(size<GLuint> viewportSize) : sceneRenderer(viewportSize)
    {
        _shadowMapSize = 512;
        _pointLightShadowMapSize = 256;

        _pointLightShadowMapDirs[0] = glm::vec3(1.0f, 0.0f, 0.0f);
        _pointLightShadowMapDirs[1] = glm::vec3(-1.0f, 0.0f, 0.0f);
        _pointLightShadowMapDirs[2] = glm::vec3(0.0f, 1.0f, 0.0f);
        _pointLightShadowMapDirs[3] = glm::vec3(0.0f, -1.0f, 0.0f);
        _pointLightShadowMapDirs[4] = glm::vec3(0.0f, 0.0f, 1.0f);
        _pointLightShadowMapDirs[5] = glm::vec3(0.0f, 0.0f, -1.0f);

        _pointLightShadowMapUps[0] = glm::vec3(0.0f, -1.0f, 0.0f);
        _pointLightShadowMapUps[1] = glm::vec3(0.0f, -1.0f, 0.0f);
        _pointLightShadowMapUps[2] = glm::vec3(0.0f, 0.0f, 1.0f);
        _pointLightShadowMapUps[3] = glm::vec3(0.0f, 0.0f, -1.0f);
        _pointLightShadowMapUps[4] = glm::vec3(0.0f, -1.0f, 0.0f);
        _pointLightShadowMapUps[5] = glm::vec3(0.0f, -1.0f, 0.0f);
    
        _biasMatrix = glm::mat4(
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f);

        initBuffers();

        createGeomPassShader();
        createStencilShader();
        createDirLightShader();
        createPointLightShader();
        createSpotLightShader();
        createShadowMapShaders();

        auto dir = phi::path::getDirectoryFullName(application::exePath);
        _randomNormalsTexture = texture::fromFile(dir + "\\" "random_normal.bmp");
        _noiseTexture = texture::fromFile(dir + "\\" + "noise.bmp");

        _ssaoActive = false;
        _ssaoBias = 0.1f;
        _ssaoIntensity = 1.2f;
        _ssaoSampleRadius = 0.07f;
        _ssaoScale = 1.5f;
        _buffersInitialized = false;

        createSSAOShader();
    }

    dsSceneRendererEx::~dsSceneRendererEx()
    {
        for (GLuint i = 0; i < _frameBuffers.size(); i++)
            DELETE(_frameBuffers[i]);
    }

    void dsSceneRendererEx::initBuffers()
    {
        _frameBuffers = std::vector<frameBuffer*>();
        _dirLightShadowMapFrameBuffers = std::vector<frameBuffer*>();

        createGeometryPassRenderTargets();
        createShadowMapsRenderTargets();
        createFinalImageRenderTargets();
        createPointLightShadowMapRenderTargets();
        //createDirLightShadowMapsRenderTargets();
    }

    void dsSceneRendererEx::createGeometryPassRenderTargets()
    {
        _frameBuffers.push_back(new frameBuffer("dsSceneRendererEx0", _viewportSize, color::transparent));
        _frameBuffers[0]->init();
        _frameBuffers[0]->bind();

        texture* t = texture::create(_viewportSize, GL_RGBA);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        renderTarget* r = _frameBuffers[0]->newRenderTarget(
            "rt0",
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0);

        _frameBuffers[0]->addRenderTarget(r);

        t = texture::create(_viewportSize, GL_RGBA16F);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        r = _frameBuffers[0]->newRenderTarget(
            "rt1", 
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT1);

        _frameBuffers[0]->addRenderTarget(r);

        t = texture::create(_viewportSize, GL_RGBA16F);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        r = _frameBuffers[0]->newRenderTarget(
            "rt2", 
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT2);

        _frameBuffers[0]->addRenderTarget(r);

        t = texture::create(_viewportSize, GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        r = _frameBuffers[0]->newRenderTarget(
            "rt3", 
            t, 
            GL_DRAW_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT);

        _frameBuffers[0]->addRenderTarget(r);

        t = renderingSystem::pickingFrameBuffer->getPickingTexture();

        r = _frameBuffers[0]->newRenderTarget(
            "rt4", 
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT3);

        _frameBuffers[0]->addRenderTarget(r);

        _frameBuffers[0]->enable(GL_CULL_FACE);
        _frameBuffers[0]->enable(GL_DEPTH_TEST);
        _frameBuffers[0]->unbind();
    }

    void dsSceneRendererEx::createDirLightShadowMapsRenderTargets()
    {   
        auto directionalLights = _scene->getDirectionalLights();
        auto directionalLightsCount = directionalLights->size();

        if (directionalLightsCount == 0)
            return;

        for (GLuint i = 0; i < directionalLightsCount; i++)
        {
            _dirLightShadowMapFrameBuffers.push_back(new frameBuffer("dsSceneRendererEx1", _viewportSize, color::transparent));
            _dirLightShadowMapFrameBuffers[i]->init();
            _dirLightShadowMapFrameBuffers[i]->bind();

            auto s = size<GLuint>(_shadowMapSize, _shadowMapSize);

            texture* t = texture::create(s, GL_RG32F);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            renderTarget* r = _dirLightShadowMapFrameBuffers[i]->newRenderTarget(
                "rt0", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0);

            _dirLightShadowMapFrameBuffers[i]->addRenderTarget(r);

            t = texture::create(s, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            r = _dirLightShadowMapFrameBuffers[i]->newRenderTarget(
                "depth", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT);

            _dirLightShadowMapFrameBuffers[i]->addRenderTarget(r);

            _dirLightShadowMapFrameBuffers[i]->enable(GL_CULL_FACE);
            _dirLightShadowMapFrameBuffers[i]->enable(GL_DEPTH_TEST);
            _dirLightShadowMapFrameBuffers[i]->unbind();
        }
    }

    void dsSceneRendererEx::createShadowMapsRenderTargets()
    {   
         _frameBuffers.push_back(new frameBuffer("dsSceneRendererEx1", _viewportSize, color::transparent));
        _frameBuffers[1]->init();
        _frameBuffers[1]->bind();

        auto s = size<GLuint>(_shadowMapSize, _shadowMapSize);

        texture* t = texture::create(s, GL_RG32F);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        renderTarget* r = _frameBuffers[1]->newRenderTarget(
            "rt0", 
            t, 
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0);

        _frameBuffers[1]->addRenderTarget(r);

        t = texture::create(s, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        r = _frameBuffers[1]->newRenderTarget(
            "depth", 
            t, 
            GL_DRAW_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT);

        _frameBuffers[1]->addRenderTarget(r);

        _frameBuffers[1]->enable(GL_CULL_FACE);
        _frameBuffers[1]->enable(GL_DEPTH_TEST);
        _frameBuffers[1]->unbind();
    }

    void dsSceneRendererEx::createPointLightShadowMapRenderTargets()
    {
        _frameBuffers.push_back(new frameBuffer("dsSceneRendererEx1", _viewportSize, color::transparent));
        _frameBuffers[3]->init();
        _frameBuffers[3]->bind();

        auto s = size<GLuint>(_pointLightShadowMapSize, _pointLightShadowMapSize);

        texture* t = texture::createCubeMap(s, GL_RG32F);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        t->setParam(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        renderTarget* r = _frameBuffers[1]->newRenderTarget(
            "rt0", 
            t, 
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0);

        _frameBuffers[3]->addRenderTarget(r);

        t = texture::create(s, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        r = _frameBuffers[3]->newRenderTarget(
            "depth", 
            t, 
            GL_DRAW_FRAMEBUFFER,
            GL_DEPTH_ATTACHMENT);

        _frameBuffers[3]->addRenderTarget(r);

        _frameBuffers[3]->enable(GL_CULL_FACE);
        _frameBuffers[3]->enable(GL_DEPTH_TEST);
        _frameBuffers[3]->unbind();
    }

    void dsSceneRendererEx::createFinalImageRenderTargets()
    {
        _frameBuffers.push_back(new frameBuffer("dsSceneRendererEx2", _viewportSize, color::transparent));
        _frameBuffers[2]->init();

        _frameBuffers[2]->bind();

        texture* t = texture::create(_viewportSize, GL_RGBA);

        t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        renderTarget* r = _frameBuffers[2]->newRenderTarget(
            "rt0", 
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0);

        _frameBuffers[2]->addRenderTarget(r);
        
        t = texture::create(_viewportSize, GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        r = _frameBuffers[2]->newRenderTarget(
            "depth", 
            t, 
            GL_DRAW_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT);

        _frameBuffers[2]->addRenderTarget(r);
        
        _frameBuffers[2]->enable(GL_CULL_FACE);
        _frameBuffers[2]->enable(GL_DEPTH_TEST);
        _frameBuffers[2]->unbind();
    }

    void dsSceneRendererEx::createGeomPassShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");

        shader* s = shaderManager::get()->loadShader("DS_GEOM_PASS_EX", "ds_geom_pass_ex.vert", "ds_geom_pass_ex.frag", attribs);

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

        s->addUniform("selectionColor");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRendererEx::createStencilShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");

        shader* s = shaderManager::get()->loadShader("DS_STENCIL_EX", "ds_stencil_ex.vert", "ds_stencil_ex.frag", attribs);

        s->addUniform("mvp");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRendererEx::createDirLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        shader* s = shaderManager::get()->loadShader("DS_DIR_LIGHT_EX", "ds_dir_light_ex.vert", "ds_dir_light_ex.frag", attribs);

        s->addUniform("v");
        s->addUniform("iv");
        s->addUniform("m");
        s->addUniform("ip");
        s->addUniform("l");

        s->addUniform("light.position");
        s->addUniform("light.color");
        s->addUniform("light.intensity");
        s->addUniform("light.direction");

        s->addUniform("rt0");
        s->addUniform("rt1");
        s->addUniform("rt2");
        s->addUniform("rt3");
        s->addUniform("shadowMap");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRendererEx::createPointLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");

        shader* s = shaderManager::get()->loadShader("DS_POINT_LIGHT_EX", "ds_point_light_ex.vert", "ds_point_light_ex.frag", attribs);

        s->addUniform("v");
        s->addUniform("iv");
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
        s->addUniform("shadowMap");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRendererEx::createSpotLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");

        shader* s = shaderManager::get()->loadShader("DS_SPOT_LIGHT_EX", "ds_spot_light_ex.vert", "ds_spot_light_ex.frag", attribs);

        s->addUniform("v");
        s->addUniform("iv");
        s->addUniform("ip");
        s->addUniform("mvp");
        s->addUniform("l");
        s->addUniform("res");

        s->addUniform("light.position");
        s->addUniform("light.color");
        s->addUniform("light.intensity");
        s->addUniform("light.oneOverRangeSqr");
        s->addUniform("light.direction");
        s->addUniform("light.cutoff");

        s->addUniform("rt0");
        s->addUniform("rt1");
        s->addUniform("rt2");
        s->addUniform("rt3");
        s->addUniform("shadowMap");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRendererEx::createShadowMapShaders()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");

        shader* s = shaderManager::get()->loadShader("SSAO_SHADOW_MAP", "ssao_shadow_map.vert", "ssao_shadow_map.frag", attribs);

        s->addUniform("mvp");
        s->addUniform("m");

        s->addUniform("nearPlane");
        s->addUniform("farPlane");
        s->addUniform("lightPos");

        shaderManager::get()->addShader(s->getName(), s);

        s = shaderManager::get()->loadShader("SSAO_SHADOW_MAP_POINT_LIGHT", "ssao_shadow_map_point_light.vert", "ssao_shadow_map_point_light.frag", attribs);

        s->addUniform("mvp");
        s->addUniform("m");

        s->addUniform("nearPlane");
        s->addUniform("farPlane");
        s->addUniform("lightPos");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRendererEx::createSSAOShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        shader* s = shaderManager::get()->loadShader("DS_SSAO", "ds_ssao.vert", "ds_ssao.frag", attribs);

        s->addUniform("ip");
        s->addUniform("m");
        s->addUniform("res");
        s->addUniform("rt1");
        s->addUniform("rt2");
        s->addUniform("rt3");
        s->addUniform("randomNormalMap");

        s->addUniform("randomSize");
        s->addUniform("sampleRadius");
        s->addUniform("intensity");
        s->addUniform("scale");
        s->addUniform("bias");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void dsSceneRendererEx::onRender()
    {
        if (!_buffersInitialized)
        {
            createDirLightShadowMapsRenderTargets();
            _buffersInitialized = true;
        }

        _viewMatrix = _scene->getActiveCamera()->getViewMatrix();
        _projMatrix = _scene->getActiveCamera()->getPerspProjMatrix();

        for (GLuint i = 0; i < _allObjectsCount; i++)
        {
            sceneObject* sceneObj = (*_allObjects)[i];

            glm::mat4 modelMatrix = sceneObj->getModelMatrix();

            _modelMatrices[sceneObj->getId()] = modelMatrix;
            _mvpMatrices[sceneObj->getId()] = _projMatrix * _viewMatrix * modelMatrix;
            _itmvMatrices[sceneObj->getId()] = glm::inverse(glm::transpose(_viewMatrix * modelMatrix));
        }

        render();
    }

    void dsSceneRendererEx::render()
    {
        _hasSelectedObjects = false;

        geomPass();

        _frameBuffers[2]->bindForDrawing();
        _frameBuffers[0]->bindForReading();
        _frameBuffers[0]->blit("rt0", 0, 0, _viewportSize.width, _viewportSize.height);
        
        pointLightPass();
        spotLightPass();
        directionalLightPass();

        glDepthMask(GL_FALSE);

        ssao();

        renderingSystem::defaultFrameBuffer->bindForDrawing();
        _frameBuffers[2]->bindForReading();
        _frameBuffers[2]->blit("rt0", 0, 0, _viewportSize.width, _viewportSize.height);
        _frameBuffers[0]->bindForReading();

        if (_hasSelectedObjects)
            selectedObjectsPass();
    }

    void dsSceneRendererEx::geomPass()
    {
        _frameBuffers[0]->bind();

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

        auto color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        glClearBufferfv(GL_COLOR, 3, &color.r); // Clears the selection render target

        shader* sh = shaderManager::get()->getShader("DS_GEOM_PASS_EX");
        sh->bind();

        glm::mat4 projectionMatrix = _camera->getPerspProjMatrix();
        glm::mat4 viewMatrix = _camera->getViewMatrix();

        for (GLuint i = 0; i < _allObjectsCount; i++)
        {
            sceneObject* sceneObj = (*_allObjects)[i];

            if(!_hasSelectedObjects)
                _hasSelectedObjects = sceneObj->getSelected();

            glm::mat4 modelMatrix = sceneObj->getModelMatrix();
            glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
            glm::mat4 mv = viewMatrix * modelMatrix;

            sh->setUniform("mv", mv);
            sh->setUniform("mvp", mvp);
            sh->setUniform("itmv", glm::inverse(glm::transpose(viewMatrix * modelMatrix)));

            sh->setUniform("ambientLightColor", _scene->getAmbientColor());

            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();

            for (GLuint j = 0; j < meshesCount; j++)
            {
                mesh* m = meshes[j];

                material* mat = m->getMaterial();

                bool selected = sceneObj->getSelected() || m->getSelected();

                sh->setUniform("selectionColor", getSelectionColor(sceneObj->getId(), m->getId(), selected));

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
        
        _frameBuffers[0]->unbind();
    }

    void dsSceneRendererEx::directionalLightPass()
    {
        auto directionalLights = _scene->getDirectionalLights();
        auto directionalLightsCount = directionalLights->size();

        if (directionalLightsCount == 0)
            return;

        glm::mat4 m = glm::mat4(
            2.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4 v = _camera->getViewMatrix();
        glm::mat4 iv = glm::inverse(v);
        glm::mat4 ip = glm::inverse(_camera->getPerspProjMatrix());

        texture* rt0Texture = _frameBuffers[0]->getRenderTarget("rt0")->getTexture();
        texture* rt1Texture = _frameBuffers[0]->getRenderTarget("rt1")->getTexture();
        texture* rt2Texture = _frameBuffers[0]->getRenderTarget("rt2")->getTexture();
        texture* rt3Texture = _frameBuffers[0]->getRenderTarget("rt3")->getTexture();

        shader* sm = shaderManager::get()->getShader("SSAO_SHADOW_MAP");
        shader* sh = shaderManager::get()->getShader("DS_DIR_LIGHT_EX");
        
        float nearPlane = 0.1f;
        float farPlane = 10.0f;
            
        glm::mat4 lp = glm::ortho<float>(-7.0, 7.0, -7.0, 7.0, nearPlane, farPlane);

        for (GLuint i = 0; i < directionalLightsCount; i++)
        {
            directionalLight* light = (*directionalLights)[i];
            
            glm::mat4 lv = light->getTransform()->getViewMatrix();
            glm::mat4 l = _biasMatrix * lp * lv;

            frameBuffer* shadowMapFrameBuffer = _dirLightShadowMapFrameBuffers[i];
            
            shadowMapFrameBuffer->bind();

            glViewport(0, 0, _shadowMapSize, _shadowMapSize);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glCullFace(GL_BACK);
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            sm->bind();

            for (GLuint i = 0; i < _allObjectsCount; i++)
            {
                sceneObject* sceneObj = (*_allObjects)[i];

                unsigned int sceneObjId = sceneObj->getId();

                glm::mat4 lmvp = lp * lv * _modelMatrices[sceneObjId];

                sm->setUniform("mvp", lmvp);
                sm->setUniform("nearPlane", nearPlane);
                sm->setUniform("farPlane", farPlane);

                std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
                auto meshesCount = meshes.size();

                for (GLuint j = 0; j < meshesCount; j++)
                    meshRenderer::render(meshes[j]);
            }

            sm->unbind();

            //texture* blurredShadowMap = blur(_frameBuffers[1]->getRenderTarget("rt0")->getTexture());

            glViewport(0, 0, _viewportSize.width, _viewportSize.height);
            
            shadowMapFrameBuffer->unbind();

            _frameBuffers[2]->bindForDrawing();

            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);

            sh->bind();
            
            sh->setUniform("m", m);
            sh->setUniform("v", v);
            sh->setUniform("iv", iv);
            sh->setUniform("ip", ip);
            sh->setUniform("l", l);
            sh->setUniform("light.position", light->getPosition());
            sh->setUniform("light.color", light->getColor());
            sh->setUniform("light.intensity", light->getIntensity());
            sh->setUniform("light.direction", light->getDirection());

            sh->setUniform("rt0", rt0Texture, 0);
            sh->setUniform("rt1", rt1Texture, 1);
            sh->setUniform("rt2", rt2Texture, 2);
            sh->setUniform("rt3", rt3Texture, 3);
            sh->setUniform("shadowMap", shadowMapFrameBuffer->getRenderTarget("rt0")->getTexture(), 4);

            meshRenderer::render(&_quad);
            
            sh->unbind();
            
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            _frameBuffers[2]->unbind();
        }
    }

    void dsSceneRendererEx::pointLightPass()
    {	
        auto pointLights = _scene->getPointLights();
        auto pointLightsCount = pointLights->size();

        if (pointLightsCount == 0)
            return;

        glm::mat4 viewMatrix = _camera->getViewMatrix();
        glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);
        glm::mat4 projectionMatrix = _camera->getPerspProjMatrix();
        glm::mat4 inverseProjectionMatrix = glm::inverse(projectionMatrix);

        texture* rt0Texture = _frameBuffers[0]->getRenderTarget("rt0")->getTexture();
        texture* rt1Texture = _frameBuffers[0]->getRenderTarget("rt1")->getTexture();
        texture* rt2Texture = _frameBuffers[0]->getRenderTarget("rt2")->getTexture();
        texture* rt3Texture = _frameBuffers[0]->getRenderTarget("rt3")->getTexture();

        glm::vec2 resolution(_viewportSize.width, _viewportSize.height);

        shader* ss = shaderManager::get()->getShader("DS_STENCIL_EX");
        shader* ps = shaderManager::get()->getShader("DS_POINT_LIGHT_EX");
        shader* sm = shaderManager::get()->getShader("SSAO_SHADOW_MAP_POINT_LIGHT");
        
        mesh* _mesh;

        for (GLuint i = 0; i < pointLightsCount; i++)
        {
            pointLight* light = (*pointLights)[i];
            
            renderTarget* shadowMapRenderTarget = _frameBuffers[3]->getRenderTarget("rt0");

            sm->bind();

            glViewport(0, 0, _pointLightShadowMapSize, _pointLightShadowMapSize);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glCullFace(GL_BACK);

            if (pointLightIndex == 6)
                pointLightIndex = 0;

            for (int i = 0; i < 6; i++)
            {
                _frameBuffers[3]->bindForDrawing(shadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);

                glClearColor(10.0, 0.0, 0.0, 0.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                float angle = PI_OVER_2;
                float nearPlane = 0.1f;
                float farPlane = light->getRange();
                glm::vec3 eye = light->getPosition();
                glm::vec3 center = eye + _pointLightShadowMapDirs[i];
                glm::vec3 up = _pointLightShadowMapUps[i];
                glm::mat4 v = glm::lookAt(eye, center, up);
                glm::mat4 p = glm::perspective(angle, 1.0f, nearPlane, farPlane);

                for (GLuint j = 0; j < _allObjectsCount; j++)
                {
                    sceneObject* sceneObj = (*_allObjects)[j];

                    unsigned int sceneObjId = sceneObj->getId();

                    glm::mat4 m = _modelMatrices[sceneObjId];
                    glm::mat4 mvp = p * v * m;

                    sm->setUniform("m", m);
                    sm->setUniform("mvp", mvp);
                    sm->setUniform("nearPlane", nearPlane);
                    sm->setUniform("farPlane", farPlane);
                    sm->setUniform("lightPos", eye);

                    std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
                    auto meshesCount = meshes.size();

                    for (GLuint k = 0; k < meshesCount; k++)
                        meshRenderer::render(meshes[k]);
                }
            }

            sm->unbind();
            
            glViewport(0, 0, _viewportSize.width, _viewportSize.height);

            _frameBuffers[3]->unbind();

            _frameBuffers[2]->bindForDrawing();
            
            glDrawBuffer(GL_COLOR_ATTACHMENT0);

            sphere* boundingVolume = light->getBoundingVolume();
            glm::mat4 modelMatrix = boundingVolume->getModelMatrix();
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

            ps->bind();

            ps->setUniform("v", viewMatrix);
            ps->setUniform("iv", inverseViewMatrix);
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
            ps->setUniform("shadowMap", shadowMapRenderTarget->getTexture(), 4);
            
            meshRenderer::render(_mesh); 

            ps->unbind();

            glCullFace(GL_BACK);
            glDisable(GL_BLEND);

            _frameBuffers[2]->unbind();
        }
    }

    void dsSceneRendererEx::spotLightPass()
    {	
        auto spotLights = _scene->getSpotLights();
        auto spotLightsCount = spotLights->size();

        if (spotLightsCount == 0)
            return;
        
        glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);

        glm::mat4 v = _camera->getViewMatrix();
        glm::mat4 iv = glm::inverse(v);
        glm::mat4 p = _camera->getPerspProjMatrix();
        glm::mat4 ip = glm::inverse(p);

        texture* rt0Texture = _frameBuffers[0]->getRenderTarget("rt0")->getTexture();
        texture* rt1Texture = _frameBuffers[0]->getRenderTarget("rt1")->getTexture();
        texture* rt2Texture = _frameBuffers[0]->getRenderTarget("rt2")->getTexture();
        texture* rt3Texture = _frameBuffers[0]->getRenderTarget("rt3")->getTexture();

        shader* ss = shaderManager::get()->getShader("DS_STENCIL_EX");
        shader* sls = shaderManager::get()->getShader("DS_SPOT_LIGHT_EX");
        shader* sm = shaderManager::get()->getShader("SSAO_SHADOW_MAP");
        
        mesh* _mesh;

        float nearPlane = 0.1f;

        for (GLuint i = 0; i < spotLightsCount; i++)
        {
            spotLight* light = (*spotLights)[i];
            
            _frameBuffers[1]->bind();
            glViewport(0, 0, _shadowMapSize, _shadowMapSize);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glCullFace(GL_BACK);
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float farPlane = light->getRadius();
            glm::mat4 lp = light->getTransform()->getProjectionMatrix(); 
            glm::mat4 lv = light->getTransform()->getViewMatrix();
            glm::mat4 l = _biasMatrix * lp * lv;

            sm->bind();

            for (GLuint i = 0; i < _allObjectsCount; i++)
            {
                sceneObject* sceneObj = (*_allObjects)[i];

                unsigned int sceneObjId = sceneObj->getId();

                glm::mat4 lmvp = lp * lv * _modelMatrices[sceneObjId];

                sm->setUniform("mvp", lmvp);
                sm->setUniform("nearPlane", nearPlane);
                sm->setUniform("farPlane", farPlane);

                std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
                auto meshesCount = meshes.size();

                for (GLuint j = 0; j < meshesCount; j++)
                    meshRenderer::render(meshes[j]);
            }

            sm->unbind();

            glViewport(0, 0, _viewportSize.width, _viewportSize.height);
            
            /*****/

            //texture* blurredShadowMap = blur(_frameBuffers[1]->getRenderTarget("rt0")->getTexture());

            _frameBuffers[1]->unbind();
            
            _frameBuffers[2]->bindForDrawing();
            glDrawBuffer(GL_COLOR_ATTACHMENT0);

            cone* boundingVolume = light->getBoundingVolume();
            glm::mat4 m = boundingVolume->getModelMatrix();
            glm::mat4 mvp = p * v * m;
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

            sls->setUniform("v", v);
            sls->setUniform("iv", iv);
            sls->setUniform("ip", ip);
            sls->setUniform("mvp", mvp);
            sls->setUniform("l", l);
            sls->setUniform("res", resolution);

            sls->setUniform("light.position", light->getPosition());
            sls->setUniform("light.color", light->getColor());
            sls->setUniform("light.intensity", light->getIntensity());
            sls->setUniform("light.oneOverRangeSqr", light->getOneOverRangerSqr());
            sls->setUniform("light.direction", light->getDirection());
            sls->setUniform("light.cutoff", light->getCutoff());

            sls->setUniform("rt0", rt0Texture, 0);
            sls->setUniform("rt1", rt1Texture, 1);
            sls->setUniform("rt2", rt2Texture, 2);
            sls->setUniform("rt3", rt3Texture, 3);
            sls->setUniform("shadowMap", _frameBuffers[1]->getRenderTarget("rt0")->getTexture(), 4);

            //TODO: Cull lights
            
            meshRenderer::render(_mesh);

            sls->unbind();

            glCullFace(GL_BACK);
            glDisable(GL_BLEND);
            //glDisable(GL_STENCIL_TEST);

            _frameBuffers[2]->unbind();
        }
    }

    void dsSceneRendererEx::selectedObjectsPass()
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);

        renderTarget* selectedRenderTarget = _frameBuffers[0]->getRenderTarget("rt4");
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

    void dsSceneRendererEx::ssao()
    {   
        _frameBuffers[2]->bindForDrawing();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_COLOR, GL_ONE);
        glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);

        texture* rt1Texture = _frameBuffers[0]->getRenderTarget("rt1")->getTexture();
        texture* rt2Texture = _frameBuffers[0]->getRenderTarget("rt2")->getTexture();
        texture* rt3Texture = _frameBuffers[0]->getRenderTarget("rt3")->getTexture();

        glm::mat4 ip = glm::inverse(_camera->getPerspProjMatrix());
        glm::vec2 res = glm::vec2(_viewportSize.width, _viewportSize.height);
        glm::mat4 m = glm::mat4(
            2.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        shader* s = shaderManager::get()->getShader("DS_SSAO");

        s->bind();
        
        s->setUniform("ip", ip);
        s->setUniform("m", m);
        s->setUniform("res", res);
        s->setUniform("rt1", rt1Texture, 0);
        s->setUniform("rt2", rt2Texture, 1);
        s->setUniform("rt3", rt3Texture, 2);
        s->setUniform("randomNormalMap", _randomNormalsTexture, 3);

        s->setUniform("randomSize", 64.0f);
        s->setUniform("sampleRadius", _ssaoSampleRadius);
        s->setUniform("intensity", _ssaoIntensity);
        s->setUniform("scale", _ssaoScale);
        s->setUniform("bias", _ssaoBias);

        meshRenderer::render(&_quad);

        s->unbind();

        glBlendEquation(GL_FUNC_ADD);
        glDisable(GL_BLEND);

        _frameBuffers[2]->unbind();
    }

    texture* dsSceneRendererEx::blur(texture* source)
    {
        _frameBuffers[2]->bindForDrawing();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::vec2 resolution = glm::vec2(1024, 1024);
        glm::mat4 modelMatrix = glm::mat4(
            2.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        shader* s = shaderManager::get()->getShader("SSAO_BLUR");

        s->bind();

        s->setUniform("m", modelMatrix);
        s->setUniform("res", resolution);
        s->setUniform("tex", source, 0);
        s->setUniform("blurScale", glm::vec2(1.0 * (1.0/resolution.x), 0.0));
        meshRenderer::render(&_quad);

        _frameBuffers[2]->unbind();

        _frameBuffers[1]->bindForDrawing();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        source = _frameBuffers[2]->getRenderTarget("rt0")->getTexture();

        s->setUniform("tex", source, 0);
        s->setUniform("blurScale", glm::vec2(0.0,  1.0 * (1.0/resolution.y)));
        meshRenderer::render(&_quad);

        s->unbind();

        _frameBuffers[1]->unbind();

        return _frameBuffers[1]->getRenderTarget("rt0")->getTexture();
    }
}