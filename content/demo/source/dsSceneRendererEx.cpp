#include "dsSceneRendererEx.h"
#include "quad.h"
#include "application.h"
#include "path.h"

namespace phi
{
    dsSceneRendererEx::dsSceneRendererEx(size<GLuint> viewportSize) : sceneRenderer(viewportSize)
    {
        _nearPlane = 0.1f;
        _farPlane = 20.0f;
        _shadowMapSize = 2048;
        _pointLightShadowMapSize = 1024;
        _pointLightIndex = 0;

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

        for (GLuint i = 0; i < _dirLightShadowMapFrameBuffers0.size(); i++)
            DELETE(_dirLightShadowMapFrameBuffers0[i]);

        for (GLuint i = 0; i < _dirLightShadowMapFrameBuffers1.size(); i++)
            DELETE(_dirLightShadowMapFrameBuffers1[i]);

        for (GLuint i = 0; i < _spotLightShadowMapFrameBuffers0.size(); i++)
            DELETE(_spotLightShadowMapFrameBuffers0[i]);

        for (GLuint i = 0; i < _spotLightShadowMapFrameBuffers1.size(); i++)
            DELETE(_spotLightShadowMapFrameBuffers1[i]);

        for (GLuint i = 0; i < _pointLightShadowMapFrameBuffers0.size(); i++)
            DELETE(_pointLightShadowMapFrameBuffers0[i]);

        for (GLuint i = 0; i < _pointLightShadowMapFrameBuffers1.size(); i++)
            DELETE(_pointLightShadowMapFrameBuffers1[i]);
    }

    void dsSceneRendererEx::initBuffers()
    {
        _frameBuffers = std::vector<frameBuffer*>();
        _dirLightShadowMapFrameBuffers0 = std::vector<frameBuffer*>();
        _dirLightShadowMapFrameBuffers1 = std::vector<frameBuffer*>();
        _spotLightShadowMapFrameBuffers0 = std::vector<frameBuffer*>();
        _spotLightShadowMapFrameBuffers1 = std::vector<frameBuffer*>();
        _pointLightShadowMapFrameBuffers0 = std::vector<frameBuffer*>();
        _pointLightShadowMapFrameBuffers1 = std::vector<frameBuffer*>();

        createGeometryPassRenderTargets();
        createFinalImageRenderTargets();
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
        
        auto s = size<GLuint>(_shadowMapSize, _shadowMapSize);

        for (GLuint i = 0; i < directionalLightsCount; i++)
        {
            _dirLightShadowMapFrameBuffers0.push_back(new frameBuffer("dsSceneRendererEx1", s, color::transparent));
            _dirLightShadowMapFrameBuffers0[i]->init();
            _dirLightShadowMapFrameBuffers0[i]->bind();

            texture* t = texture::create(s, GL_RG32F);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            renderTarget* r = _dirLightShadowMapFrameBuffers0[i]->newRenderTarget(
                "rt0", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0);

            _dirLightShadowMapFrameBuffers0[i]->addRenderTarget(r);

            t = texture::create(s, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            r = _dirLightShadowMapFrameBuffers0[i]->newRenderTarget(
                "depth", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT);

            _dirLightShadowMapFrameBuffers0[i]->addRenderTarget(r);

            _dirLightShadowMapFrameBuffers0[i]->enable(GL_CULL_FACE);
            _dirLightShadowMapFrameBuffers0[i]->enable(GL_DEPTH_TEST);
            _dirLightShadowMapFrameBuffers0[i]->unbind();
        }

        for (GLuint i = 0; i < directionalLightsCount; i++)
        {
            _dirLightShadowMapFrameBuffers1.push_back(new frameBuffer("dsSceneRendererEx1", s, color::transparent));
            _dirLightShadowMapFrameBuffers1[i]->init();
            _dirLightShadowMapFrameBuffers1[i]->bind();

            texture* t = texture::create(s, GL_RG32F);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            renderTarget* r = _dirLightShadowMapFrameBuffers1[i]->newRenderTarget(
                "rt0", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0);

            _dirLightShadowMapFrameBuffers1[i]->addRenderTarget(r);

            t = texture::create(s, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            r = _dirLightShadowMapFrameBuffers1[i]->newRenderTarget(
                "depth", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT);

            _dirLightShadowMapFrameBuffers1[i]->addRenderTarget(r);

            _dirLightShadowMapFrameBuffers1[i]->enable(GL_CULL_FACE);
            _dirLightShadowMapFrameBuffers1[i]->enable(GL_DEPTH_TEST);
            _dirLightShadowMapFrameBuffers1[i]->unbind();
        }
    }

    void dsSceneRendererEx::createSpotLightShadowMapsRenderTargets()
    {   
        auto spotLights = _scene->getSpotLights();
        auto spotLightsCount = spotLights->size();

        if (spotLightsCount == 0)
            return;
        
        auto s = size<GLuint>(_shadowMapSize, _shadowMapSize);

        for (GLuint i = 0; i < spotLightsCount; i++)
        {
            _spotLightShadowMapFrameBuffers0.push_back(new frameBuffer("dsSceneRendererEx1", s, color::transparent));
            _spotLightShadowMapFrameBuffers0[i]->init();
            _spotLightShadowMapFrameBuffers0[i]->bind();

            texture* t = texture::create(s, GL_RG32F);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            renderTarget* r = _spotLightShadowMapFrameBuffers0[i]->newRenderTarget(
                "rt0", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0);

            _spotLightShadowMapFrameBuffers0[i]->addRenderTarget(r);

            t = texture::create(s, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            r = _spotLightShadowMapFrameBuffers0[i]->newRenderTarget(
                "depth", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT);

            _spotLightShadowMapFrameBuffers0[i]->addRenderTarget(r);

            _spotLightShadowMapFrameBuffers0[i]->enable(GL_CULL_FACE);
            _spotLightShadowMapFrameBuffers0[i]->enable(GL_DEPTH_TEST);
            _spotLightShadowMapFrameBuffers0[i]->unbind();
        }

        for (GLuint i = 0; i < spotLightsCount; i++)
        {
            _spotLightShadowMapFrameBuffers1.push_back(new frameBuffer("dsSceneRendererEx1", s, color::transparent));
            _spotLightShadowMapFrameBuffers1[i]->init();
            _spotLightShadowMapFrameBuffers1[i]->bind();

            texture* t = texture::create(s, GL_RG32F);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            renderTarget* r = _spotLightShadowMapFrameBuffers1[i]->newRenderTarget(
                "rt0", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0);

            _spotLightShadowMapFrameBuffers1[i]->addRenderTarget(r);

            t = texture::create(s, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            r = _spotLightShadowMapFrameBuffers1[i]->newRenderTarget(
                "depth", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT);

            _spotLightShadowMapFrameBuffers1[i]->addRenderTarget(r);

            _spotLightShadowMapFrameBuffers1[i]->enable(GL_CULL_FACE);
            _spotLightShadowMapFrameBuffers1[i]->enable(GL_DEPTH_TEST);
            _spotLightShadowMapFrameBuffers1[i]->unbind();
        }
    }

    void dsSceneRendererEx::createPointLightShadowMapsRenderTargets()
    {
        auto pointLights = _scene->getPointLights();
        auto pointLightsCount = pointLights->size();

        if (pointLightsCount == 0)
            return;
        
        auto s = size<GLuint>(_pointLightShadowMapSize, _pointLightShadowMapSize);

        for (GLuint i = 0; i < pointLightsCount; i++)
        {
            _pointLightShadowMapFrameBuffers0.push_back(new frameBuffer("dsSceneRendererEx1", s, color::transparent));
            _pointLightShadowMapFrameBuffers0[i]->init();
            _pointLightShadowMapFrameBuffers0[i]->bind();

            texture* t = texture::createCubeMap(s, GL_RG32F);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            t->setParam(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            renderTarget* r = _pointLightShadowMapFrameBuffers0[i]->newRenderTarget(
                "rt0", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0);

            _pointLightShadowMapFrameBuffers0[i]->addRenderTarget(r);

            t = texture::createCubeMap(s, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            t->setParam(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            r = _pointLightShadowMapFrameBuffers0[i]->newRenderTarget(
                "depth", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT);

            _pointLightShadowMapFrameBuffers0[i]->addRenderTarget(r);

            _pointLightShadowMapFrameBuffers0[i]->enable(GL_CULL_FACE);
            _pointLightShadowMapFrameBuffers0[i]->enable(GL_DEPTH_TEST);
            _pointLightShadowMapFrameBuffers0[i]->unbind();

            _pointLightShadowMapFrameBuffers1.push_back(new frameBuffer("dsSceneRendererEx1", s, color::transparent));
            _pointLightShadowMapFrameBuffers1[i]->init();
            _pointLightShadowMapFrameBuffers1[i]->bind();

            t = texture::createCubeMap(s, GL_RG32F);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            t->setParam(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            r = _pointLightShadowMapFrameBuffers1[i]->newRenderTarget(
                "rt0", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0);

            _pointLightShadowMapFrameBuffers1[i]->addRenderTarget(r);

            t = texture::createCubeMap(s, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
            t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            t->setParam(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            r = _pointLightShadowMapFrameBuffers1[i]->newRenderTarget(
                "depth", 
                t, 
                GL_DRAW_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT);

            _pointLightShadowMapFrameBuffers1[i]->addRenderTarget(r);

            _pointLightShadowMapFrameBuffers1[i]->enable(GL_CULL_FACE);
            _pointLightShadowMapFrameBuffers1[i]->enable(GL_DEPTH_TEST);
            _pointLightShadowMapFrameBuffers1[i]->unbind();
        }
    }

    void dsSceneRendererEx::createFinalImageRenderTargets()
    {
        _frameBuffers.push_back(new frameBuffer("dsSceneRendererEx2", _viewportSize, color::transparent));
        _frameBuffers[1]->init();

        _frameBuffers[1]->bind();

        texture* t = texture::create(_viewportSize, GL_RGBA);

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
        
        t = texture::create(_viewportSize, GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV);
        t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        r = _frameBuffers[1]->newRenderTarget(
            "depth", 
            t, 
            GL_DRAW_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT);

        _frameBuffers[1]->addRenderTarget(r);
        
        _frameBuffers[1]->enable(GL_CULL_FACE);
        _frameBuffers[1]->enable(GL_DEPTH_TEST);
        _frameBuffers[1]->unbind();
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

        _geometryPassShader = s;
    }

    void dsSceneRendererEx::createStencilShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");

        shader* s = shaderManager::get()->loadShader("DS_STENCIL_EX", "ds_stencil_ex.vert", "ds_stencil_ex.frag", attribs);

        s->addUniform("mvp");

        shaderManager::get()->addShader(s->getName(), s);

        _stencilShader = s;
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

        _dirLightShader = s;
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

        _pointLightShader = s;
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

        _spotLightShader = s;
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

        _shadowMapShader = s;   

        s = shaderManager::get()->loadShader("SSAO_SHADOW_MAP_POINT_LIGHT", "ssao_shadow_map_point_light.vert", "ssao_shadow_map_point_light.frag", attribs);

        s->addUniform("mvp");
        s->addUniform("m");

        s->addUniform("nearPlane");
        s->addUniform("farPlane");
        s->addUniform("lightPos");

        shaderManager::get()->addShader(s->getName(), s); 

        _pointLightShadowMapShader = s;
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

        _ssaoShader = s;
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
        glClearBufferfv(GL_COLOR, 3, &color.r);

        _geometryPassShader->bind();

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

            _geometryPassShader->setUniform("mv", mv);
            _geometryPassShader->setUniform("mvp", mvp);
            _geometryPassShader->setUniform("itmv", glm::inverse(glm::transpose(viewMatrix * modelMatrix)));
            _geometryPassShader->setUniform("ambientLightColor", _scene->getAmbientColor());

            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();

            for (GLuint j = 0; j < meshesCount; j++)
            {
                mesh* m = meshes[j];

                material* mat = m->getMaterial();

                bool selected = sceneObj->getSelected() || m->getSelected();

                _geometryPassShader->setUniform("selectionColor", getSelectionColor(sceneObj->getId(), m->getId(), selected));

                _geometryPassShader->setUniform("mat.ambientColor", mat->getAmbientColor());
                _geometryPassShader->setUniform("mat.diffuseColor", mat->getDiffuseColor());
                _geometryPassShader->setUniform("mat.specularColor", mat->getSpecularColor());
                _geometryPassShader->setUniform("mat.ka", mat->getKa());
                _geometryPassShader->setUniform("mat.kd", mat->getKd());
                _geometryPassShader->setUniform("mat.ks", mat->getKs());
                _geometryPassShader->setUniform("mat.shininess", mat->getShininess());

                _geometryPassShader->setUniform("diffuseMap", mat->getDiffuseTexture(), 0);
                _geometryPassShader->setUniform("normalMap", mat->getNormalTexture(), 1);
                _geometryPassShader->setUniform("specularMap", mat->getSpecularTexture(), 2);

                meshRenderer::render(m);
            }
        }

        _geometryPassShader->unbind();

        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        
        _frameBuffers[0]->unbind();

        _rt0Texture = _frameBuffers[0]->getRenderTarget("rt0")->getTexture();
        _rt1Texture = _frameBuffers[0]->getRenderTarget("rt1")->getTexture();
        _rt2Texture = _frameBuffers[0]->getRenderTarget("rt2")->getTexture();
        _rt3Texture = _frameBuffers[0]->getRenderTarget("rt3")->getTexture();
    }

    void dsSceneRendererEx::shadowMapPasses()
    {
        auto directionalLights = _scene->getDirectionalLights();
        auto directionalLightsCount = directionalLights->size();

        auto spotLights = _scene->getSpotLights();
        auto spotLightsCount = spotLights->size();

        if (directionalLightsCount == 0 && spotLightsCount == 0)
            return;
            
        glm::mat4 lp;
        glm::mat4 lv;

        glViewport(0, 0, _shadowMapSize, _shadowMapSize);

        _shadowMapShader->bind();
        
        if (_redrawStaticShadowMaps)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glCullFace(GL_BACK);
            glClearColor(0.0, 0.0, 0.0, 0.0);

            for (GLuint i = 0; i < directionalLightsCount; i++)
            {
                auto light = (*directionalLights)[i];

                lp = glm::ortho<float>(-7.0, 7.0, -7.0, 7.0, _nearPlane, _farPlane);
                lv = light->getTransform()->getViewMatrix();

                staticShadowMapPass(_dirLightShadowMapFrameBuffers0[i], lp * lv, _nearPlane, _farPlane);

                light->setShadowMap(_dirLightShadowMapFrameBuffers0[i]->getRenderTarget("rt0")->getTexture());
            }
            
            for (GLuint i = 0; i < spotLightsCount; i++)
            {
                auto light = (*spotLights)[i];

                lp = light->getTransform()->getProjectionMatrix();
                lv = light->getTransform()->getViewMatrix();

                staticShadowMapPass(_spotLightShadowMapFrameBuffers0[i], lp * lv, _nearPlane, light->getRadius());

                light->setShadowMap(_spotLightShadowMapFrameBuffers0[i]->getRenderTarget("rt0")->getTexture());
            }

            _redrawStaticShadowMaps = false;
            
            glDisable(GL_DEPTH_TEST);
        }

        if (_dynamicObjects.size() > 0)
        {
            for (GLuint i = 0; i < directionalLightsCount; i++)
            {
                auto light = (*directionalLights)[i];

                lp = glm::ortho<float>(-7.0, 7.0, -7.0, 7.0, _nearPlane, _farPlane);
                lv = light->getTransform()->getViewMatrix();
                
                dynamicShadowMapPass(_dirLightShadowMapFrameBuffers0[i], _dirLightShadowMapFrameBuffers1[i], lp * lv, _nearPlane, _farPlane);

                light->setShadowMap(_dirLightShadowMapFrameBuffers1[i]->getRenderTarget("rt0")->getTexture());
            }   

            for (GLuint i = 0; i < spotLightsCount; i++)
            {
                auto light = (*spotLights)[i];

                lp = light->getTransform()->getProjectionMatrix();
                lv = light->getTransform()->getViewMatrix();
                
                dynamicShadowMapPass(_spotLightShadowMapFrameBuffers0[i], _spotLightShadowMapFrameBuffers1[i], lp * lv, _nearPlane, light->getRadius());

                light->setShadowMap(_spotLightShadowMapFrameBuffers1[i]->getRenderTarget("rt0")->getTexture());
            }
        }   

        _shadowMapShader->unbind();
        
        glViewport(0, 0, _viewportSize.width, _viewportSize.height);
    }

    void dsSceneRendererEx::staticShadowMapPass(frameBuffer* staticFrameBuffer, glm::mat4 l, float n, float f)
    {
        staticFrameBuffer->bindForDrawing();

        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (std::map<GLuint, sceneObject*>::iterator it = _staticObjects.begin(); it != _staticObjects.end(); ++it)
        {              
            unsigned int sceneObjId = it->first;
            sceneObject* sceneObj = it->second;
            
            _shadowMapShader->setUniform("mvp", l * _modelMatrices[sceneObjId]);
            _shadowMapShader->setUniform("nearPlane", n);
            _shadowMapShader->setUniform("farPlane", f);

            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();

            for (GLuint j = 0; j < meshesCount; j++)
                meshRenderer::render(meshes[j]);
        }

        staticFrameBuffer->unbind();
    }

    void dsSceneRendererEx::dynamicShadowMapPass(frameBuffer* staticFrameBuffer, frameBuffer* dynamicFrameBuffer, glm::mat4 l, float n, float f)
    {
        dynamicFrameBuffer->bindForDrawing();
        staticFrameBuffer->bindForReading();

        staticFrameBuffer->blit("rt0", 0, 0, _shadowMapSize, _shadowMapSize, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        for (std::map<GLuint, sceneObject*>::iterator it = _dynamicObjects.begin(); it != _dynamicObjects.end(); ++it)
        {              
            unsigned int sceneObjId = it->first;
            sceneObject* sceneObj = it->second;
            
            _shadowMapShader->setUniform("mvp", l * _modelMatrices[sceneObjId]);
            _shadowMapShader->setUniform("nearPlane", n);
            _shadowMapShader->setUniform("farPlane", f);

            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();

            for (GLuint j = 0; j < meshesCount; j++)
                meshRenderer::render(meshes[j]);
        }

        glDisable(GL_DEPTH_TEST);

        dynamicFrameBuffer->unbind();
    }

    /*LOG(std::to_string(mvp[0].x) + " | " + std::to_string(mvp[0].y) + " | " + std::to_string(mvp[0].z) + " | " + std::to_string(mvp[0].w));
                    LOG(std::to_string(mvp[1].x) + " | " + std::to_string(mvp[1].y) + " | " + std::to_string(mvp[1].z) + " | " + std::to_string(mvp[1].w));
                    LOG(std::to_string(mvp[2].x) + " | " + std::to_string(mvp[2].y) + " | " + std::to_string(mvp[2].z) + " | " + std::to_string(mvp[2].w));
                    LOG(std::to_string(mvp[3].x) + " | " + std::to_string(mvp[3].y) + " | " + std::to_string(mvp[3].z) + " | " + std::to_string(mvp[3].w));
                    LOG("---------------------------------------------------------------");
*/

    void dsSceneRendererEx::pointLightShadowMapPasses()
    {
        auto pointLights = _scene->getPointLights();
        auto pointLightsCount = pointLights->size();
        
        if (pointLightsCount == 0)
            return;
            
        glm::mat4 lp;
        glm::mat4 lv;

        glViewport(0, 0, _pointLightShadowMapSize, _pointLightShadowMapSize);
        
        _pointLightShadowMapShader->bind();

        if (_redrawStaticShadowMaps)
        {   
        
            for (GLuint l = 0; l < pointLightsCount; l++)
            {
                pointLight* light = (*pointLights)[l];
                frameBuffer* staticFramebuffer = _pointLightShadowMapFrameBuffers0[l];
                renderTarget* shadowMapRenderTarget = staticFramebuffer->getRenderTarget("rt0");

                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                glDepthMask(GL_TRUE);
                glCullFace(GL_BACK);
                
                float farPlane = light->getRange();
                glm::vec3 eye = light->getPosition();

                for (int i = 0; i < 6; i++)
                {
                    staticFramebuffer->bindForDrawing(shadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);

                    glClearColor(0.0, 0.0, 0.0, 0.0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    
                    glm::vec3 center = eye + _pointLightShadowMapDirs[i];
                    glm::vec3 up = _pointLightShadowMapUps[i];
                    glm::mat4 v = glm::lookAt(eye, center, up);
                    glm::mat4 p = glm::perspective(PI_OVER_2, 1.0f, _nearPlane, farPlane);

                    /*for (GLuint i = 0; i < _allObjectsCount; i++)
                    {
                        sceneObject* sceneObj = (*_allObjects)[i];
                        unsigned int sceneObjId = sceneObj->getId();*/

                    for (std::map<GLuint, sceneObject*>::iterator it = _staticObjects.begin(); it != _staticObjects.end(); ++it)
                    { 
                        unsigned int sceneObjId = it->first;
                        sceneObject* sceneObj = it->second;

                        glm::mat4 m = _modelMatrices[sceneObjId];
                        glm::mat4 mvp = p * v * m;

                        _pointLightShadowMapShader->setUniform("m", m);
                        _pointLightShadowMapShader->setUniform("mvp", mvp);
                        _pointLightShadowMapShader->setUniform("nearPlane", _nearPlane);
                        _pointLightShadowMapShader->setUniform("farPlane", farPlane);
                        _pointLightShadowMapShader->setUniform("lightPos", eye);

                        std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
                        auto meshesCount = meshes.size();

                        for (GLuint k = 0; k < meshesCount; k++)
                            meshRenderer::render(meshes[k]);
                    }

                    staticFramebuffer->unbind();
                }
                
                glDisable(GL_DEPTH_TEST);

                light->setShadowMap(shadowMapRenderTarget->getTexture());
            }
        }

        if (_dynamicObjects.size() > 0)
        {
            for (GLuint l = 0; l < pointLightsCount; l++)
            {
                //GLuint l = 0;

                pointLight* light = (*pointLights)[l];
                frameBuffer* staticFrameBuffer = _pointLightShadowMapFrameBuffers0[l];
                frameBuffer* dynamicFrameBuffer = _pointLightShadowMapFrameBuffers1[l];

                renderTarget* staticShadowMapRenderTarget = staticFrameBuffer->getRenderTarget("rt0");
                renderTarget* dynamicShadowMapRenderTarget = dynamicFrameBuffer->getRenderTarget("rt0");
                
                float farPlane = 50;
                glm::vec3 eye = light->getPosition();
                glm::mat4 p = glm::perspective(PI_OVER_2, 1.0f, _nearPlane, farPlane);
                
                for (int i = 0; i < 6; i++)
                {
                    //unsigned int i = 0;

                    glm::vec3 center = eye + _pointLightShadowMapDirs[i];
                    glm::vec3 up = _pointLightShadowMapUps[i];
                    glm::mat4 v = glm::lookAt(eye, center, up);
                    
                    dynamicFrameBuffer->bindForDrawing(dynamicShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
                    staticFrameBuffer->bindForReading(staticShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
                    staticFrameBuffer->blita(0, 0, _pointLightShadowMapSize, _pointLightShadowMapSize, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
                    
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LESS);
                    glDepthMask(GL_TRUE);

                    for (std::map<GLuint, sceneObject*>::iterator it = _dynamicObjects.begin(); it != _dynamicObjects.end(); ++it)
                    {              
                        unsigned int sceneObjId = it->first;
                        sceneObject* sceneObj = it->second;
            
                        glm::mat4 m = _modelMatrices[sceneObjId];
                        glm::mat4 mvp = p * v * m;

                        _pointLightShadowMapShader->setUniform("m", m);
                        _pointLightShadowMapShader->setUniform("mvp", mvp);
                        _pointLightShadowMapShader->setUniform("nearPlane", _nearPlane);
                        _pointLightShadowMapShader->setUniform("farPlane", farPlane);
                        _pointLightShadowMapShader->setUniform("lightPos", eye);

                        std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
                        auto meshesCount = meshes.size();

                        for (GLuint k = 0; k < meshesCount; k++)
                            meshRenderer::render(meshes[k]);
                    }

                    glDisable(GL_DEPTH_TEST);
                    
                    dynamicFrameBuffer->unbind();
                }
                

                light->setShadowMap(dynamicShadowMapRenderTarget->getTexture());
            }
        }
        
        _pointLightShadowMapShader->unbind();
        glViewport(0, 0, _viewportSize.width, _viewportSize.height);
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

        for (GLuint i = 0; i < directionalLightsCount; i++)
        {
            directionalLight* light = (*directionalLights)[i];
            glm::mat4 lp = glm::ortho<float>(-7.0, 7.0, -7.0, 7.0, _nearPlane, _farPlane);
            glm::mat4 lv = light->getTransform()->getViewMatrix();
            glm::mat4 l = _biasMatrix * lp * lv;

            _frameBuffers[1]->bindForDrawing();

            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);

            _dirLightShader->bind();
            
            _dirLightShader->setUniform("m", m);
            _dirLightShader->setUniform("v", v);
            _dirLightShader->setUniform("iv", iv);
            _dirLightShader->setUniform("ip", ip);
            _dirLightShader->setUniform("l", l);
            _dirLightShader->setUniform("light.position", light->getPosition());
            _dirLightShader->setUniform("light.color", light->getColor());
            _dirLightShader->setUniform("light.intensity", light->getIntensity());
            _dirLightShader->setUniform("light.direction", light->getDirection());

            _dirLightShader->setUniform("rt0", _rt0Texture, 0);
            _dirLightShader->setUniform("rt1", _rt1Texture, 1);
            _dirLightShader->setUniform("rt2", _rt2Texture, 2);
            _dirLightShader->setUniform("rt3", _rt3Texture, 3);
            _dirLightShader->setUniform("shadowMap", light->getShadowMap(), 4);

            meshRenderer::render(&_quad);
            
            _dirLightShader->unbind();
            
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            _frameBuffers[1]->unbind();
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

        mesh* _mesh;

        float nearPlane = 0.1f;

        _frameBuffers[1]->bindForDrawing();

        for (GLuint i = 0; i < spotLightsCount; i++)
        {
            spotLight* light = (*spotLights)[i];

            glm::mat4 lp = light->getTransform()->getProjectionMatrix();
            glm::mat4 lv = light->getTransform()->getViewMatrix();
            glm::mat4 l = _biasMatrix * lp * lv;
            
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

            _stencilShader->bind();
            _stencilShader->setUniform("mvp", mvp);
            meshRenderer::render(_mesh); 
            _stencilShader->unbind();

            glDrawBuffer(GL_COLOR_ATTACHMENT0);

            glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            
            _spotLightShader->bind();

            _spotLightShader->setUniform("v", v);
            _spotLightShader->setUniform("iv", iv);
            _spotLightShader->setUniform("ip", ip);
            _spotLightShader->setUniform("mvp", mvp);
            _spotLightShader->setUniform("l", l);
            _spotLightShader->setUniform("res", resolution);

            _spotLightShader->setUniform("light.position", light->getPosition());
            _spotLightShader->setUniform("light.color", light->getColor());
            _spotLightShader->setUniform("light.intensity", light->getIntensity());
            _spotLightShader->setUniform("light.oneOverRangeSqr", light->getOneOverRangerSqr());
            _spotLightShader->setUniform("light.direction", light->getDirection());
            _spotLightShader->setUniform("light.cutoff", light->getCutoff());

            _spotLightShader->setUniform("rt0", _rt0Texture, 0);
            _spotLightShader->setUniform("rt1", _rt1Texture, 1);
            _spotLightShader->setUniform("rt2", _rt2Texture, 2);
            _spotLightShader->setUniform("rt3", _rt3Texture, 3);
            _spotLightShader->setUniform("shadowMap", _spotLightShadowMapFrameBuffers1[i]->getRenderTarget("rt0")->getTexture(), 4);

            //TODO: Cull lights
            
            meshRenderer::render(_mesh);

            _spotLightShader->unbind();

            glCullFace(GL_BACK);
            glDisable(GL_BLEND);
        }
        
        _frameBuffers[1]->unbind();
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

        glm::vec2 resolution(_viewportSize.width, _viewportSize.height);

        mesh* _mesh;

        for (GLuint i = 0; i < pointLightsCount; i++)
        {
            pointLight* light = (*pointLights)[i];
            
            /*renderTarget* shadowMapRenderTarget = _frameBuffers[3]->getRenderTarget("rt0");

            _pointLightShadowMapShader->bind();

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

                    _pointLightShadowMapShader->setUniform("m", m);
                    _pointLightShadowMapShader->setUniform("mvp", mvp);
                    _pointLightShadowMapShader->setUniform("nearPlane", nearPlane);
                    _pointLightShadowMapShader->setUniform("farPlane", farPlane);
                    _pointLightShadowMapShader->setUniform("lightPos", eye);

                    std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
                    auto meshesCount = meshes.size();

                    for (GLuint k = 0; k < meshesCount; k++)
                        meshRenderer::render(meshes[k]);
                }
            }

            _pointLightShadowMapShader->unbind();
            
            glViewport(0, 0, _viewportSize.width, _viewportSize.height);

            _frameBuffers[3]->unbind();*/

            _frameBuffers[1]->bindForDrawing();
            
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

            _stencilShader->bind();
            _stencilShader->setUniform("mvp", mvp);
            meshRenderer::render(_mesh); 
            _stencilShader->unbind();

            glDrawBuffer(GL_COLOR_ATTACHMENT0);

            glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);

            _pointLightShader->bind();

            _pointLightShader->setUniform("v", viewMatrix);
            _pointLightShader->setUniform("iv", inverseViewMatrix);
            _pointLightShader->setUniform("m", modelMatrix);
            _pointLightShader->setUniform("ip", inverseProjectionMatrix);
            _pointLightShader->setUniform("mvp", mvp);
            _pointLightShader->setUniform("res", resolution);

            _pointLightShader->setUniform("light.position", light->getPosition());
            _pointLightShader->setUniform("light.color", light->getColor());
            _pointLightShader->setUniform("light.intensity", light->getIntensity());
            _pointLightShader->setUniform("light.range", light->getRange());
            _pointLightShader->setUniform("light.oneOverRangeSqr", light->getOneOverRangerSqr());
            _pointLightShader->setUniform("rt0", _rt0Texture, 0);
            _pointLightShader->setUniform("rt1", _rt1Texture, 1);
            _pointLightShader->setUniform("rt2", _rt2Texture, 2);
            _pointLightShader->setUniform("rt3", _rt3Texture, 3);
            _pointLightShader->setUniform("shadowMap", light->getShadowMap(), 4);
            //_pointLightShader->setUniform("shadowMap", _pointLightShadowMapFrameBuffers1[i]->getRenderTarget("rt0")->getTexture(), 4);
            meshRenderer::render(_mesh); 

            _pointLightShader->unbind();

            glCullFace(GL_BACK);
            glDisable(GL_BLEND);

            _frameBuffers[1]->unbind();
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
        _frameBuffers[1]->bindForDrawing();

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

        _frameBuffers[1]->unbind();
    }

    texture* dsSceneRendererEx::blur(texture* source)
    {
        _frameBuffers[1]->bindForDrawing();
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

        _frameBuffers[1]->unbind();

        _frameBuffers[1]->bindForDrawing();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        source = _frameBuffers[1]->getRenderTarget("rt0")->getTexture();

        s->setUniform("tex", source, 0);
        s->setUniform("blurScale", glm::vec2(0.0,  1.0 * (1.0/resolution.y)));
        meshRenderer::render(&_quad);

        s->unbind();

        _frameBuffers[1]->unbind();

        return _frameBuffers[1]->getRenderTarget("rt0")->getTexture();
    }

    void dsSceneRendererEx::render()
    {
        _hasSelectedObjects = false;

        _staticObjects = _scene->getStaticObjects();
        _dynamicObjects = _scene->getDynamicObjects();

        geomPass();
 
        shadowMapPasses();
        pointLightShadowMapPasses();

        if (_redrawStaticShadowMaps)
            _redrawStaticShadowMaps = false;

        _frameBuffers[1]->bindForDrawing();
        _frameBuffers[0]->bindForReading();
        _frameBuffers[0]->blit("rt0", 0, 0, _viewportSize.width, _viewportSize.height);
        
        pointLightPass();
        spotLightPass();
        directionalLightPass();

        glDepthMask(GL_FALSE);

        ssao();

        renderingSystem::defaultFrameBuffer->bindForDrawing();
        _frameBuffers[1]->bindForReading();
        _frameBuffers[1]->blit("rt0", 0, 0, _viewportSize.width, _viewportSize.height);
        
        //_dirLightShadowMapFrameBuffers1[0]->bindForReading();
        //_dirLightShadowMapFrameBuffers1[0]->blit("rt0", 0, 0, 400, 400);
        
        
        /*unsigned int s = 128;
        unsigned int dx = 0;
        unsigned int dy = 0;
        unsigned int l = 0;

        
        renderTarget* staticShadowMapRenderTarget = _pointLightShadowMapFrameBuffers0[l]->getRenderTarget("rt0");
        renderTarget* dynamicShadowMapRenderTarget = _pointLightShadowMapFrameBuffers1[l]->getRenderTarget("rt0");

        _pointLightShadowMapFrameBuffers0[l]->bindForReading(staticShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5);
        _pointLightShadowMapFrameBuffers0[l]->blita(0 + dx, s + dy, s, s);
        _pointLightShadowMapFrameBuffers0[l]->bindForReading(staticShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1);
        _pointLightShadowMapFrameBuffers0[l]->blita(s + dx, s + dy, s, s);
        _pointLightShadowMapFrameBuffers0[l]->bindForReading(staticShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4);
        _pointLightShadowMapFrameBuffers0[l]->blita(2 * s + dx, s + dy, s, s);
        _pointLightShadowMapFrameBuffers0[l]->bindForReading(staticShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0);
        _pointLightShadowMapFrameBuffers0[l]->blita(3 * s + dx, s + dy, s, s);
        _pointLightShadowMapFrameBuffers0[l]->bindForReading(staticShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3);
        _pointLightShadowMapFrameBuffers0[l]->blita(s * 2 + dx, s * 2 + dy, s, s);
        _pointLightShadowMapFrameBuffers0[l]->bindForReading(staticShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2);
        _pointLightShadowMapFrameBuffers0[l]->blita(s * 2 + dx, 0 + dy, s, s);
        
        dx = 512;*/
        /*
        _pointLightShadowMapFrameBuffers1[l]->bindForReading(dynamicShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5);
        _pointLightShadowMapFrameBuffers1[l]->blita(0 + dx, s + dy, s, s);
        _pointLightShadowMapFrameBuffers1[l]->bindForReading(dynamicShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1);
        _pointLightShadowMapFrameBuffers1[l]->blita(s + dx, s + dy, s, s);
        _pointLightShadowMapFrameBuffers1[l]->bindForReading(dynamicShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4);
        _pointLightShadowMapFrameBuffers1[l]->blita(2 * s + dx, s + dy, s, s);
        _pointLightShadowMapFrameBuffers1[l]->bindForReading(dynamicShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 0);
        _pointLightShadowMapFrameBuffers1[l]->blita(3 * s + dx, s + dy, s, s);
        _pointLightShadowMapFrameBuffers1[l]->bindForReading(dynamicShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3);
        _pointLightShadowMapFrameBuffers1[l]->blita(s * 2 + dx, s * 2 + dy, s, s);
        _pointLightShadowMapFrameBuffers1[l]->bindForReading(dynamicShadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2);
        _pointLightShadowMapFrameBuffers1[l]->blita(s * 2 + dx, 0 + dy, s, s);*/

        
        //_dirLightShadowMapFrameBuffers1[0]->bindForReading();
        //_dirLightShadowMapFrameBuffers1[0]->blit("rt0", _shadowMapSize/2, 0, _shadowMapSize/2, _shadowMapSize/2);
        
        //_dirLightShadowMapFrameBuffers0[0]->bindForReading();
        //_dirLightShadowMapFrameBuffers0[0]->blit("depth", 0, _shadowMapSize/2, _shadowMapSize/2, _shadowMapSize/2);
        
        //_dirLightShadowMapFrameBuffers1[0]->bindForReading();
        //_dirLightShadowMapFrameBuffers1[0]->blit("depth", _shadowMapSize/2, _shadowMapSize/2, _shadowMapSize/2, _shadowMapSize/2);
        
        _frameBuffers[0]->bindForReading();

        //if (_hasSelectedObjects)
            //selectedObjectsPass();
    }
    
    void dsSceneRendererEx::onRender()
    {
        if (!_buffersInitialized)
        {
            createDirLightShadowMapsRenderTargets();
            createSpotLightShadowMapsRenderTargets();
            createPointLightShadowMapsRenderTargets();
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
}