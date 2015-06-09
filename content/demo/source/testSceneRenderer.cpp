#include "testSceneRenderer.h"
#include "renderingSystem.h"
#include "skyDome.h"

namespace phi
{
	testSceneRenderer::testSceneRenderer(size<GLuint> viewportSize) : sceneRenderer(viewportSize)
	{
		_frameBuffers.push_back(new frameBuffer("testSceneRenderer0", viewportSize, color::transparent));
		_frameBuffers[0]->init();
        _frameBuffers[0]->bind();

		createDefaultRenderTargets();

		_frameBuffers[0]->bind();
		_frameBuffers[0]->enable(GL_CULL_FACE);
		_frameBuffers[0]->enable(GL_DEPTH_TEST);
		_frameBuffers[0]->unbind();

        _frameBuffers.push_back(new frameBuffer("testSceneRenderer1", viewportSize, color::transparent));
		_frameBuffers[1]->init();
        _frameBuffers[1]->bind();

		createShadowMapsRenderTargets();

		_frameBuffers[1]->bind();
		_frameBuffers[1]->enable(GL_CULL_FACE);
		_frameBuffers[1]->enable(GL_DEPTH_TEST);
		_frameBuffers[1]->unbind();
        
        _frameBuffers.push_back(new frameBuffer("testSceneRenderer2", viewportSize, color::transparent));
		_frameBuffers[2]->init();
        _frameBuffers[2]->bind();

		createFXRenderTargets();

		_frameBuffers[2]->bind();
		_frameBuffers[2]->enable(GL_CULL_FACE);
		_frameBuffers[2]->enable(GL_DEPTH_TEST);
		_frameBuffers[2]->unbind();

        createPointLightShadowMapRenderTargets();

		createGeomPassShader();
        createDirLightShaders();
        createPointLightShader();
        createSpotLightShader();
        createShadowMapsShaders();
		createSSAOShader();
        createReflectionsShader();
        createRefractionsShader();
        createBlurShader();
        createSkyDomeShader();

        _randomNormalsTexture = texture::fromFile("random_normal.bmp");
        _noiseTexture = texture::fromFile("noise.bmp");

        _ssaoActive = false;
        _ssaoBias = 0.1f;
        _ssaoIntensity = 1.2f;
        _ssaoSampleRadius = 0.07f;
        _ssaoScale = 1.5f;

        _skyDome = new skyDome(5.0f, 100, 100, nullptr);
	}
	
	testSceneRenderer::~testSceneRenderer()
	{
	}

	void testSceneRenderer::createDefaultRenderTargets()
	{
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

        t = texture::create(_viewportSize, GL_RGBA16F);
		t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		r = _frameBuffers[0]->newRenderTarget(
            "rt3",
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT3);

		_frameBuffers[0]->addRenderTarget(r);

		t = texture::create(_viewportSize, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);
		t->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		t->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		r = _frameBuffers[0]->newRenderTarget(
			"depth", 
			t, 
			GL_DRAW_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT);

		_frameBuffers[0]->addRenderTarget(r);

        _frameBuffers[0]->unbind();
	}

    void testSceneRenderer::createShadowMapsRenderTargets()
    {   
        _frameBuffers[1]->bind();

        auto s = size<GLuint>(4096, 4096);

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

        _frameBuffers[1]->unbind();
    }

    void testSceneRenderer::createPointLightShadowMapRenderTargets()
    {
        _frameBuffers.push_back(new frameBuffer("testSceneRenderer3", _viewportSize, color::transparent));
		_frameBuffers[3]->init();
        _frameBuffers[3]->bind();

        auto s = size<GLuint>(512, 512);

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

    void testSceneRenderer::createFXRenderTargets()
	{
        _frameBuffers[2]->bind();
        
        auto s = size<GLuint>(4096, 4096);
		texture* t = texture::create(_viewportSize, GL_RGBA);

		t->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		t->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		t->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		t->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		renderTarget* r = _frameBuffers[0]->newRenderTarget(
            "rt0", 
            t,
            GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0);

		_frameBuffers[2]->addRenderTarget(r);

        _frameBuffers[2]->unbind();
	}

	void testSceneRenderer::createGeomPassShader()
	{
		std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");

        shader* s = shaderManager::get()->loadShader("SSAO_GEOM_PASS", "ssao_geom_pass.vert", "ssao_geom_pass.frag", attribs);

        s->addUniform("mv");
        s->addUniform("mvp");
        s->addUniform("itmv");

        s->addUniform("ambientLightColor");
        
        s->addUniform("diffuseMap");
        s->addUniform("emissiveMap");
        s->addUniform("normalMap");
        s->addUniform("specularMap");

        s->addUniform("mat.ambientColor");
        s->addUniform("mat.emissiveColor");
        s->addUniform("mat.ka");
        s->addUniform("mat.isEmissive");
        s->addUniform("mat.reflectivity");

        shaderManager::get()->addShader(s->getName(), s);
	}

    void testSceneRenderer::createDirLightShaders()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");

        shader* s = shaderManager::get()->loadShader("SSAO_DIR_LIGHT", "ssao_dir_light.vert", "ssao_dir_light.frag", attribs);

        s->addUniform("p");
        s->addUniform("v");
        s->addUniform("m");
        s->addUniform("mvp");
        s->addUniform("itmv");
        s->addUniform("lmvp");

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
        s->addUniform("shadowMap");

        shaderManager::get()->addShader(s->getName(), s);

        attribs.clear();
        attribs.push_back("inPosition");

        s = shaderManager::get()->loadShader("SSAO_SHADOW_MAP", "ssao_shadow_map.vert", "ssao_shadow_map.frag", attribs);

        s->addUniform("mvp");
        s->addUniform("m");
        
        s->addUniform("nearPlane");
        s->addUniform("farPlane");
        s->addUniform("lightPos");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void testSceneRenderer::createPointLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");

        shader* s = shaderManager::get()->loadShader("SSAO_POINT_LIGHT", "ssao_point_light.vert", "ssao_point_light.frag", attribs);

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
        s->addUniform("emissiveMap");
        s->addUniform("shadowMap");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void testSceneRenderer::createSpotLightShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");

        shader* s = shaderManager::get()->loadShader("SSAO_SPOT_LIGHT", "ssao_spot_light.vert", "ssao_spot_light.frag", attribs);

        s->addUniform("p");
        s->addUniform("v");
        s->addUniform("m");
        s->addUniform("mvp");
        s->addUniform("itmv");
        s->addUniform("lmvp");

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
        s->addUniform("emissiveMap");
        s->addUniform("shadowMap");
        s->addUniform("randomNormalMap");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void testSceneRenderer::createShadowMapsShaders()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        
        shader* s = shaderManager::get()->loadShader("SSAO_SHADOW_MAP_POINT_LIGHT", "ssao_shadow_map_point_light.vert", "ssao_shadow_map_point_light.frag", attribs);

        s->addUniform("mvp");
        s->addUniform("m");
        
        s->addUniform("nearPlane");
        s->addUniform("farPlane");
        s->addUniform("lightPos");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void testSceneRenderer::createSSAOShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");

		shader* s = shaderManager::get()->loadShader("SSAO_SSAO", "ssao_ssao.vert", "ssao_ssao.frag", attribs);

		s->addUniform("m");
		s->addUniform("res");
		s->addUniform("positionMap");
		s->addUniform("normalMap");
        s->addUniform("randomNormalMap");

        s->addUniform("randomSize");
        s->addUniform("sampleRadius");
        s->addUniform("intensity");
        s->addUniform("scale");
        s->addUniform("bias");

		shaderManager::get()->addShader(s->getName(), s);
	}

    void testSceneRenderer::createReflectionsShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");

		shader* s = shaderManager::get()->loadShader("SSAO_REFLECTIONS", "ssao_reflections.vert", "ssao_reflections.frag", attribs);

		s->addUniform("p");
		s->addUniform("m");
		s->addUniform("res");
        s->addUniform("colorMap");
		s->addUniform("positionMap");
		s->addUniform("normalMap");
        s->addUniform("depthMap");
        s->addUniform("reflectivityMap");
        
		shaderManager::get()->addShader(s->getName(), s);
	}

    void testSceneRenderer::createRefractionsShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");
		attribs.push_back("inNormal");
		attribs.push_back("inTangent");

		shader* s = shaderManager::get()->loadShader("SSAO_REFRACTIONS", "ssao_refractions.vert", "ssao_refractions.frag", attribs);

        s->addUniform("p");
        s->addUniform("v");
        s->addUniform("m");
        s->addUniform("mvp");
        s->addUniform("itmv");
		s->addUniform("res");

        s->addUniform("colorMap");
		s->addUniform("positionMap");
		s->addUniform("normalMap");
        s->addUniform("depthMap");
        s->addUniform("objectNormalMap");
        
        s->addUniform("mat.refractionIndex");

		shaderManager::get()->addShader(s->getName(), s);
	}

    void testSceneRenderer::createBlurShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");

		shader* s = shaderManager::get()->loadShader("SSAO_BLUR", "ssao_blur.vert", "ssao_blur.frag", attribs);

		s->addUniform("m");
		s->addUniform("res");
		s->addUniform("tex");
        s->addUniform("blurScale");

		shaderManager::get()->addShader(s->getName(), s);
	}

    void testSceneRenderer::createSkyDomeShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");

        shader* s = shaderManager::get()->loadShader("SKY_DOME", "sky_dome.vert", "sky_dome.frag", attribs);

        s->addUniform("p");
        s->addUniform("v");
        s->addUniform("m");
        s->addUniform("mvp");
        s->addUniform("itmv");
        s->addUniform("time");
        s->addUniform("sunOrientation");
        s->addUniform("radius");
        s->addUniform("horizonColor");

        shaderManager::get()->addShader(s->getName(), s);
    }

    void testSceneRenderer::geomPass()
    {
        _frameBuffers[0]->bindForDrawing();

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

        shader* sh = shaderManager::get()->getShader("SSAO_GEOM_PASS");
        sh->bind();
        
        sh->setUniform("ambientLightColor", _scene->getAmbientColor());
        
        for (GLuint i = 0; i < _allObjectsCount; i++)
        {
            sceneObject* sceneObj = (*_allObjects)[i];

            glm::mat4 modelMatrix = _modelMatrices[sceneObj->getId()];
            glm::mat4 mvp = _mvpMatrices[sceneObj->getId()];
            glm::mat4 itmv = _itmvMatrices[sceneObj->getId()];

            sh->setUniform("mv", _viewMatrix * modelMatrix);
            sh->setUniform("mvp", mvp);
            sh->setUniform("itmv", itmv);

            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();

            for (GLuint j = 0; j < meshesCount; j++)
		    {
                mesh* m = meshes[j];

                material* mat = m->getMaterial();

                //if (mat->getReflectivity() > 0.0f)
                //    continue;

                sh->setUniform("diffuseMap", mat->getDiffuseTexture(), 0);
                sh->setUniform("normalMap", mat->getNormalTexture(), 1);
                sh->setUniform("emissiveMap", mat->getEmissiveTexture(), 2);
                sh->setUniform("specularMap", mat->getSpecularTexture(), 3);

                sh->setUniform("mat.ambientColor", mat->getAmbientColor());
                sh->setUniform("mat.emissiveColor", mat->getEmissiveColor());
                sh->setUniform("mat.ka", mat->getKa());
                sh->setUniform("mat.isEmissive", mat->getIsEmissive());
                sh->setUniform("mat.reflectivity", mat->getReflectivity());

                meshRenderer::render(m);
            }
        }

        sh->unbind();

        _frameBuffers[0]->unbind();
    }

    void testSceneRenderer::dirLightPass(directionalLight* light)
    {
        glm::vec3 camPos = _camera->getPosition();
        glm::vec3 camDir = _camera->getDirection();

        float nearPlane = 0.1;
        float farPlane = 50.0;

        glm::mat4 p = glm::ortho<float>(-15.0, 15.0, -15.0, 15.0, nearPlane, farPlane);

        glm::mat4 biasMatrix = glm::mat4(
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f);

        _frameBuffers[1]->bind();
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0,0,4096,4096);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glCullFace(GL_BACK);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _camera->setPosition(light->getPosition());
        _camera->setDirection(light->getDirection());
        _camera->update();

        shader* sm = shaderManager::get()->getShader("SSAO_SHADOW_MAP");
        sm->bind();
            
        glm::mat4 v = _camera->getViewMatrix();

        std::map<unsigned int, glm::mat4> lmvps;

        for (GLuint i = 0; i < _allObjectsCount; i++)
        {
            sceneObject* sceneObj = (*_allObjects)[i];
                
            unsigned int sceneObjId = sceneObj->getId();

            glm::mat4 m = _modelMatrices[sceneObjId];
            glm::mat4 mvp = p * v * m;

            lmvps[sceneObjId] = mvp;

            sm->setUniform("mvp", mvp);
            sm->setUniform("nearPlane", nearPlane);
            sm->setUniform("farPlane", farPlane);

            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();
            
            for (GLuint j = 0; j < meshesCount; j++)
                meshRenderer::render(meshes[j]);
        }

        sm->unbind();
            
        _camera->setPosition(camPos);
        _camera->setDirection(camDir);
        _camera->update();
        
        _frameBuffers[1]->unbind();

        //texture* blurredShadowMap = blur(_frameBuffers[1]->getRenderTarget("rt0")->getTexture());

        glPopAttrib();

        _frameBuffers[0]->bindForDrawing();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
            
        glCullFace(GL_BACK);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        shader* sh = shaderManager::get()->getShader("SSAO_DIR_LIGHT");
        sh->bind();

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_EQUAL);

        for (GLuint i = 0; i < _allObjectsCount; i++)
        {
            sceneObject* sceneObj = (*_allObjects)[i];

            unsigned int sceneObjId = sceneObj->getId();

            glm::mat4 modelMatrix = _modelMatrices[sceneObjId];
            glm::mat4 mvp = _mvpMatrices[sceneObjId];
            glm::mat4 itmv = _itmvMatrices[sceneObjId];
            glm::mat4 lmvp = biasMatrix * lmvps[sceneObjId];

            sh->setUniform("p", _projMatrix);
            sh->setUniform("v", _viewMatrix);
            sh->setUniform("m", modelMatrix);
            sh->setUniform("mvp", mvp);
            sh->setUniform("itmv", itmv);
            sh->setUniform("lmvp", lmvp);

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
                sh->setUniform("shadowMap", _frameBuffers[1]->getRenderTarget("rt0")->getTexture(), 4);

                meshRenderer::render(m);
            }
        }

        sh->unbind();

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        _frameBuffers[1]->unbind();

        glDisable(GL_BLEND);
    }

    void testSceneRenderer::dirLightPasses()
    {
        auto directionalLights = _scene->getDirectionalLights();
        auto directionalLightsCount = directionalLights->size();

        if (directionalLightsCount == 0)
            return;
        
        for (GLuint l = 0; l< directionalLightsCount; l++)
            dirLightPass((*directionalLights)[l]);
    }

    void testSceneRenderer::pointLightPasses()
    {
        auto pointLights = _scene->getPointLights();
        auto pointLightsCount = pointLights->size();

        if (pointLightsCount == 0)
            return;

        glm::vec3 camPos = _camera->getPosition();
        glm::vec3 camDir = _camera->getDirection();

        glm::mat4 biasMatrix = glm::mat4(
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f);

        glm::vec3 pointLightShadowMapDirs[6] = 
        {  
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
        };

        glm::vec3 pointLightShadowMapUps[6] = 
        {  
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
        };

        for (GLuint l = 0; l< pointLightsCount; l++)
        {
            pointLight* light = (*pointLights)[l];

            renderTarget* shadowMapRenderTarget = _frameBuffers[3]->getRenderTarget("rt0");
            
            shader* sm = shaderManager::get()->getShader("SSAO_SHADOW_MAP_POINT_LIGHT");
            sm->bind();
            
            glPushAttrib(GL_VIEWPORT_BIT);
            glViewport(0,0,512,512);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glCullFace(GL_BACK);

            for (int i = 0; i < 6; i++)
            {
                _frameBuffers[3]->bindForDrawing(shadowMapRenderTarget, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
            
                glClearColor(10.0, 0.0, 0.0, 0.0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
                float angle = PI_OVER_2;
                float nearPlane = 0.1f;
                float farPlane = light->getRange();
                glm::vec3 eye = light->getPosition();
                glm::vec3 center = eye + pointLightShadowMapDirs[i];
                glm::vec3 up = pointLightShadowMapUps[i];
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
            
            _frameBuffers[3]->unbind();

            //texture* blurredShadowMap = blur(_frameBuffers[1]->getRenderTarget("rt0")->getTexture());
            //texture* blurredShadowMap = blur(_frameBuffers[1]->getRenderTarget("rt0")->getTexture());

            glPopAttrib();

            _frameBuffers[0]->bindForDrawing();
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
            glCullFace(GL_BACK);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            shader* sh = shaderManager::get()->getShader("SSAO_POINT_LIGHT");

            sh->bind();

            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            for (GLuint i = 0; i < _allObjectsCount; i++)
            {
                sceneObject* sceneObj = (*_allObjects)[i];
                auto sceneObjId = sceneObj->getId();
                glm::mat4 modelMatrix = _modelMatrices[sceneObjId];
                glm::mat4 mvp = _mvpMatrices[sceneObjId];
                glm::mat4 itmv = _itmvMatrices[sceneObjId];

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
                    sh->setUniform("shadowMap", _frameBuffers[3]->getRenderTarget("rt0")->getTexture(), 4);

                    meshRenderer::render(m);
                }
            }

            sh->unbind();

            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            
            //glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
            _frameBuffers[0]->unbind();
            glDisable(GL_BLEND);
        }
    }

    void testSceneRenderer::spotLightPasses()
    {
        auto spotLights = _scene->getSpotLights();
        auto spotLightsCount = spotLights->size();

        if (spotLightsCount == 0)
            return;

        glm::vec3 camPos = _camera->getPosition();
        glm::vec3 camDir = _camera->getDirection();

        glm::mat4 biasMatrix = glm::mat4(
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f);

        for (GLuint l = 0; l< spotLightsCount; l++)
        {
            spotLight* light = (*spotLights)[l];
            
            _frameBuffers[1]->bind();
            glPushAttrib(GL_VIEWPORT_BIT);
            glViewport(0,0,4096,4096);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
            glCullFace(GL_BACK);
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            _camera->setPosition(light->getPosition());
            _camera->setDirection(light->getDirection());
            _camera->update();
            
            //float angle = glm::clamp(PI * light->getCutoff() + 0.2f, 0.0001f, PI - 0.0001f);
            float angle = glm::acos(light->getCutoff()) * 2.0;
            float nearPlane = 0.1f;
            float farPlane = light->getRadius();
            glm::mat4 p = glm::perspective(angle, 1.0f, nearPlane, farPlane);   
            
            //glm::mat4 p = glm::ortho<float>(-15.0, 15.0, -15.0, 15.0, 0.0, 50.0);
            glm::mat4 v = _camera->getViewMatrix();

            std::map<unsigned int, glm::mat4> lmvps;
            
            shader* sm = shaderManager::get()->getShader("SSAO_SHADOW_MAP");
            sm->bind();

            for (GLuint i = 0; i < _allObjectsCount; i++)
            {
                sceneObject* sceneObj = (*_allObjects)[i];
                
                unsigned int sceneObjId = sceneObj->getId();

                glm::mat4 m = _modelMatrices[sceneObjId];
                glm::mat4 mvp = p * v * m;

                lmvps[sceneObjId] = mvp;

                sm->setUniform("mvp", mvp);
                sm->setUniform("nearPlane", nearPlane);
                sm->setUniform("farPlane", farPlane);

                std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
                auto meshesCount = meshes.size();
            
                for (GLuint j = 0; j < meshesCount; j++)
                    meshRenderer::render(meshes[j]);
            }

            sm->unbind();
            
            _camera->setPosition(camPos);
            _camera->setDirection(camDir);
            _camera->update();
        
            _frameBuffers[1]->unbind();

            //texture* blurredShadowMap = blur(_frameBuffers[1]->getRenderTarget("rt0")->getTexture());

            glPopAttrib();

            _frameBuffers[0]->bindForDrawing();
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            
            glCullFace(GL_BACK);
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE, GL_ONE);

            shader* sh = shaderManager::get()->getShader("SSAO_SPOT_LIGHT");

            sh->bind();

            glDepthMask(GL_FALSE);
            glDepthFunc(GL_EQUAL);

            for (GLuint i = 0; i < _allObjectsCount; i++)
            {
                sceneObject* sceneObj = (*_allObjects)[i];
                unsigned int sceneObjId = sceneObj->getId();

                glm::mat4 modelMatrix = _modelMatrices[sceneObjId];
                glm::mat4 mvp = _mvpMatrices[sceneObjId];
                glm::mat4 itmv = _itmvMatrices[sceneObjId];
                glm::mat4 lmvp = biasMatrix * lmvps[sceneObjId];

                sh->setUniform("p", _projMatrix);
                sh->setUniform("v", _viewMatrix);
                sh->setUniform("m", modelMatrix);
                sh->setUniform("mvp", mvp);
                sh->setUniform("itmv", itmv);
                sh->setUniform("lmvp", lmvp);

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
                    sh->setUniform("emissiveMap", mat->getEmissiveTexture(), 3);
                    sh->setUniform("shadowMap", _frameBuffers[1]->getRenderTarget("rt0")->getTexture(), 4);
                    sh->setUniform("randomNormalMap", _randomNormalsTexture, 5);

                    meshRenderer::render(m);
                }
            }

            sh->unbind();

            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);

            _frameBuffers[0]->unbind();

            glDisable(GL_BLEND);
        }
    }

    void testSceneRenderer::ssao()
    {   
        _frameBuffers[0]->bindForDrawing();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_COLOR, GL_ONE);
        glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);

        texture* positionMap = _frameBuffers[0]->getRenderTarget("rt1")->getTexture();
        texture* normalMap = _frameBuffers[0]->getRenderTarget("rt2")->getTexture();
        glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);
        glm::mat4 modelMatrix = glm::mat4(
            2.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        shader* s = shaderManager::get()->getShader("SSAO_SSAO");

        s->bind();
        
        s->setUniform("m", modelMatrix);
        s->setUniform("res", resolution);
        s->setUniform("positionMap", positionMap, 0);
        s->setUniform("normalMap", normalMap, 1);
        s->setUniform("randomNormalMap", _randomNormalsTexture, 2);

        s->setUniform("randomSize", 64.0f);
        s->setUniform("sampleRadius", _ssaoSampleRadius);
        s->setUniform("intensity", _ssaoIntensity);
        s->setUniform("scale", _ssaoScale);
        s->setUniform("bias", _ssaoBias);
        
        meshRenderer::render(&_quad);

        s->unbind();
        
        glBlendEquation(GL_FUNC_ADD);
        glDisable(GL_BLEND);

        _frameBuffers[0]->unbind();
    }
    
    void testSceneRenderer::reflections()
    {   
        _frameBuffers[2]->bindForDrawing();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_COLOR_BUFFER_BIT);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        texture* colorMap = _frameBuffers[0]->getRenderTarget("rt0")->getTexture();
        texture* positionMap = _frameBuffers[0]->getRenderTarget("rt1")->getTexture();
        texture* normalMap = _frameBuffers[0]->getRenderTarget("rt2")->getTexture();
        texture* reflectivityMap = _frameBuffers[0]->getRenderTarget("rt3")->getTexture();
        texture* depthMap = _frameBuffers[0]->getRenderTarget("depth")->getTexture();

        glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);
        
        glm::mat4 modelMatrix = glm::mat4(
            2.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        shader* s = shaderManager::get()->getShader("SSAO_REFLECTIONS");

        s->bind();
        
        s->setUniform("p", _projMatrix);
        s->setUniform("m", modelMatrix);
        s->setUniform("res", resolution);

        s->setUniform("colorMap", colorMap, 0);
        s->setUniform("positionMap", positionMap, 1);
        s->setUniform("normalMap", normalMap, 2);
        s->setUniform("depthMap", depthMap, 3);
        s->setUniform("reflectivityMap", reflectivityMap, 4);
        
        meshRenderer::render(&_quad);

        s->unbind();
        
        //glDisable(GL_BLEND);

        _frameBuffers[2]->unbind();
    }

    void testSceneRenderer::refractions()
    {   
        _frameBuffers[2]->bindForDrawing();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_COLOR_BUFFER_BIT);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        texture* colorMap = _frameBuffers[0]->getRenderTarget("rt0")->getTexture();
        texture* positionMap = _frameBuffers[0]->getRenderTarget("rt1")->getTexture();
        texture* normalMap = _frameBuffers[0]->getRenderTarget("rt2")->getTexture();
        texture* depthMap = _frameBuffers[0]->getRenderTarget("depth")->getTexture();

        glm::vec2 resolution = glm::vec2(_viewportSize.width, _viewportSize.height);

        shader* s = shaderManager::get()->getShader("SSAO_REFRACTIONS");

        s->bind();
        
        s->setUniform("res", resolution);
        
        s->setUniform("colorMap", colorMap, 0);
        s->setUniform("positionMap", positionMap, 1);
        s->setUniform("normalMap", normalMap, 2);
        s->setUniform("depthMap", depthMap, 3);
        
        for (GLuint i = 0; i < _allObjectsCount; i++)
        {
            sceneObject* sceneObj = (*_allObjects)[i];

            glm::mat4 modelMatrix = _modelMatrices[sceneObj->getId()];
            glm::mat4 mvp = _mvpMatrices[sceneObj->getId()];
            glm::mat4 itmv = _itmvMatrices[sceneObj->getId()];
            
            s->setUniform("p", _projMatrix);
            s->setUniform("v", _viewMatrix);
            s->setUniform("m", modelMatrix);
            s->setUniform("mvp", mvp);
            s->setUniform("itmv", itmv);

            std::vector<mesh*> meshes = sceneObj->getModel()->getMeshes();
            auto meshesCount = meshes.size();

            for (GLuint j = 0; j < meshesCount; j++)
		    {
                mesh* m = meshes[j];

                material* mat = m->getMaterial();

                //if (mat->getReflectivity() == 0.0)
                    //continue;
                
                s->setUniform("objectNormalMap", mat->getNormalTexture(), 4);
                s->setUniform("mat.refractionIndex", mat->getReflectivity());
                meshRenderer::render(m);
            }
        }

        s->unbind();
        
        //glDisable(GL_BLEND);

        _frameBuffers[2]->unbind();
    }

    texture* testSceneRenderer::blur(texture* tex)
    {
        _frameBuffers[2]->bindForDrawing();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::vec2 resolution = glm::vec2(4096, 4096);
        glm::mat4 modelMatrix = glm::mat4(
            2.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        shader* s = shaderManager::get()->getShader("SSAO_BLUR");

        s->bind();
        
        s->setUniform("m", modelMatrix);
        s->setUniform("res", resolution);
        s->setUniform("tex", tex, 0);
        s->setUniform("blurScale", glm::vec2(1.0 * (1.0/resolution.x), 0.0));
        meshRenderer::render(&_quad);

        _frameBuffers[2]->unbind();

        _frameBuffers[1]->bindForDrawing();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        tex = _frameBuffers[2]->getRenderTarget("rt0")->getTexture();

        s->setUniform("tex", tex, 0);
        s->setUniform("blurScale", glm::vec2(0.0,  1.0 * (1.0/resolution.y)));
        meshRenderer::render(&_quad);

        s->unbind();

        _frameBuffers[1]->unbind();

        return _frameBuffers[1]->getRenderTarget("rt0")->getTexture();
    }

    void testSceneRenderer::renderSkyDome()
    {
        _skyDome->setPosition(_camera->getPosition());
        _skyDome->update();

        _frameBuffers[0]->bindForDrawing();
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_EQUAL);
        glEnable(GL_DEPTH_TEST);
        
        shader* sh = shaderManager::get()->getShader("SKY_DOME");
        sh->bind();

        _camera->update();

        auto camPos = _camera->getPosition();

        glm::mat4 scale = glm::mat4(
            1000.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1000.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1000.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4 translation = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            camPos.x, camPos.y, camPos.z, 1.0f);

        glm::mat4 modelMatrix = translation * scale;

        //glm::mat4 modelMatrix= scale;
        //glm::mat4 modelMatrix = _skyDome->getTransform()->getModelMatrix();
        //glm::mat4 modelMatrix = glm::mat4(1.0);

        sh->setUniform("p", _projMatrix);
        sh->setUniform("v", _viewMatrix);
        sh->setUniform("m", modelMatrix);
        sh->setUniform("mvp", _projMatrix * _viewMatrix * modelMatrix);
        sh->setUniform("itmv", glm::inverse(glm::transpose(_viewMatrix * modelMatrix)));
        sh->setUniform("time", _skyDome->getDayTime());
        sh->setUniform("sunOrientation", glm::radians(_skyDome->getSunOrientation()));
        sh->setUniform("horizonColor", _skyDome->getHorizonColor());
        sh->setUniform("radius", _skyDome->getRadius());
        
		glCullFace(GL_FRONT);
		meshRenderer::render(_skyDome->getModel()->getMeshes()[0]);
		glCullFace(GL_BACK);

        sh->unbind();

        _frameBuffers[0]->unbind();

        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

    void testSceneRenderer::onRender()
	{
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
        
        geomPass();

        dirLightPasses();

        //dirLightPass(_skyDome->getSun());

        pointLightPasses();
        spotLightPasses();

        glDepthMask(GL_FALSE);
        
        if (_ssaoActive)
        {
            ssao();
            //refractions();
            reflections();

            //ssao();

            renderingSystem::defaultFrameBuffer->bindForDrawing();
            _frameBuffers[2]->bindForReading();
            _frameBuffers[2]->blit("rt0", 0, 0, _viewportSize.width, _viewportSize.height);
            _frameBuffers[0]->bindForReading();
        
        }
        else
        {
        
            renderingSystem::defaultFrameBuffer->bindForDrawing();
            _frameBuffers[0]->bindForReading();
            _frameBuffers[0]->blit("rt0", 0, 0, _viewportSize.width, _viewportSize.height);
        }
	}
}