#include <precompiled.h>
#include "screen.h"

#include <diagnostics\stopwatch.h>

#include <core\multiCommand.h>

#include <loader\importer.h>

#include <animation\floatAnimator.h>

#include <scenes\unselectSceneObjectCommand.h>
#include <scenes\groupSceneObjectsCommand.h>
#include <scenes\deleteSceneObjectCommand.h>

#include <apps\application.h>
#include <apps\undoCommand.h>
#include <apps\redoCommand.h>

#include <ui\control.h>
#include <ui\text.h>

#include "addObjectCommand.h"

#include <rendering\renderPass.h>
#include <rendering\shader.h>

#include <scenes\sceneId.h>

using namespace phi;

namespace demon
{
    screen::screen(string name, uint width, uint height) :
        window(name, width, height)
    {
    }

    screen::~screen()
    {
        //safeDelete(_designContext);
    }

    void screen::onInit()
    {
        initGL();
        initLibraries();
        initScene();
        initUi();
        initInput();

#ifdef _DEBUG
        _messageQueue = new blockingQueue<phi::watcherMessage>();
        _watcher = new watcher(application::resourcesPath + "/shaders", _messageQueue, [&](string shaderFileName)
        {
            auto fileExtension = path::getExtension(shaderFileName);
            if (fileExtension == phi::shadersManager::FRAG_EXT ||
                fileExtension == phi::shadersManager::VERT_EXT)
            {
                auto shaderName = path::getFileNameWithoutExtension(shaderFileName);
                _gl->shadersManager->reloadShader(shaderName);
            }
        });
        _watcher->startWatch();
#endif
    }

    void screen::initGL()
    {
        application::logInfo("Initializing OpenGl");

        auto initState = gl::state();
        initState.clearColor = vec4(1.0f);
        initState.frontFace = gl::frontFace::ccw;
        initState.culling = true;
        initState.cullFace = gl::cullFace::back;
        initState.depthMask = true;
        initState.depthTest = true;
        initState.useBindlessTextures = false;
        initState.useSparseTextures = false;
        initState.swapInterval = 1;

        auto info = gl::glInfo();
        info.state = initState;
        info.shadersPath = application::resourcesPath + "/shaders";
        info.fontsPath = application::resourcesPath + "/fonts";
        _gl = new gl(info);

        application::logInfo("Vendor: " + _gl->getVendor() + ".");
        application::logInfo("Renderer: " + _gl->getRenderer() + ".");
        application::logInfo("Version: " + _gl->getVersion() + ".");

        application::logInfo("Extensions:");
        for (auto extensionStatus : _gl->extensions)
        {
            auto status = extensionStatus.second ? " [Ok]" : " [Not Ok]";
            application::logInfo(extensionStatus.first + status);
        }
    }

    void screen::initLibraries()
    {
        _userLibrary = new library(application::libraryPath);
        _userLibrary->load();

        _projectLibrary = new library(application::path);
    }

    void screen::initScene()
    {
        _scene = new scene(_gl, static_cast<float>(_width), static_cast<float>(_height));
        auto camera = _scene->getCamera();

        auto cameraTransform = camera->getTransform();
        auto cameraPos = vec3(0.0f, 0.0f, 10.0f);
        cameraTransform->setLocalPosition(cameraPos);
        cameraTransform->setDirection(-cameraPos);

        auto floor = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        _scene->add(floor);

        auto chair0 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair0->getTransform()->setLocalPosition(vec3(0.f, .5f, .0f));

        auto chair1 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair1->getTransform()->setLocalPosition(vec3(2.0f, .5f, 0.f));

        auto chair2 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair2->getTransform()->setLocalPosition(vec3(0.f, .5f, 2.0f));

        auto chair3 = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair3->getTransform()->setLocalPosition(vec3(2.0f, .5f, 2.0f));
/*
        _scene->add(chair0);
        _scene->add(chair1);
        _scene->add(chair2);
        _scene->add(chair3);*/
    }

    void screen::initUi()
    {
        camera* uiCamera = new camera("uiCamera", static_cast<float>(_width), static_cast<float>(_height), 0.1f, 10000.0f, PI_OVER_4);

        _ui = new ui(uiCamera, _scene->getRenderer(), _gl, static_cast<float>(_width), static_cast<float>(_height));

        auto font = _gl->fontsManager->load("Roboto-Thin.ttf", 24);
        auto fontFps = _gl->fontsManager->load("Roboto-Thin.ttf", 12);

        auto labelNandinho = _ui->newLabel(L"nanddiiiiiiiinho", vec3(-100.0f, 0.0f, 0.0f));
        auto control = labelNandinho->getComponent<phi::control>();
        control->setColor(color::fromRGBA(0.9f, 0.9f, 0.9f, 1.0f));
        control->setIsGlassy(false);

        auto text = labelNandinho->getComponent<phi::text>();
        text->setFont(font);
        text->setColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

        _labelFps = _ui->newLabel(L"Fps: ", vec3(-200.f, 100.f, 0.f));
        auto fpsControl = _labelFps->getComponent<phi::control>();
        fpsControl->setColor(color::fromRGBA(.7f, .5f, .9f, 1.0f));
        fpsControl->setIsGlassy(false);

        auto textFps = _labelFps->getComponent<phi::text>();
        textFps->setFont(fontFps);
        textFps->setColor(color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        //auto controlsRenderDataBuffer = new buffer<controlRenderData>(bufferTarget::shader);
        //controlsRenderDataBuffer->data(sizeof(controlRenderData), nullptr, bufferDataUsage::dynamicDraw);
        ////bindBufferBase(0) ?


        //vector<string> attribs;
        //attribs.push_back("inPosition");
        //attribs.push_back("inTexCoord");
        //attribs.push_back("inModelMatrix");
        //auto shader = _gl->shadersManager->load("control", attribs);

        //shader->addUniform(0, "v");
        //shader->addUniform(1, "p");
        //shader->addUniform(2, "textureArrays");

        //auto controlRenderPass = new renderPass(shader);

        //controlRenderPass->setOnRender([&](phi::shader* shader)
        //{
        //    controlsRenderDataBuffer->bindBufferBase(0);

        //    glDisable(GL_DEPTH_TEST);
        //    glDisable(GL_CULL_FACE);
        //    glEnable(GL_BLEND);
        //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //    glBlendColor(1, 1, 1, 1);

        //    shader->bind();
        //    shader->setUniform(0, _camera->getViewMatrix());
        //    shader->setUniform(1, _camera->getProjectionMatrix());
        //    shader->setUniform(2, _texturesManager->units);

        //    glBindVertexArray(_vao);
        //    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, _instanceCount);
        //    glBindVertexArray(0);

        //    shader->unbind();
        //    glBlendColor(0, 0, 0, 0);
        //    glDisable(GL_BLEND);
        //    glEnable(GL_DEPTH_TEST);
        //    glEnable(GL_CULL_FACE);
        //});


        ////auto textRenderPass = new renderPass();

        //auto nandinhoLayer = new layer({ controlRenderPass });
        //nandinhoLayer->setOnNodeAdded([&](node* node)
        //{
        //    node->traverse<phi::control>([&](phi::control* control)
        //    {
        //        auto texture = pipeline::getTextureFromImage(control->getBackgroundImage(), _gl->defaultAlbedoImage);
        //        textureAddress address = _gl->texturesManager->get(texture);
        //        auto renderData = controlRenderData();
        //        renderData.backgroundColor = control->getBackgroundColor();
        //        renderData.backgroundTextureUnit = address.unit;
        //        renderData.backgroundTexturePage = address.page;
        //        if (!control->getIsGlassy())
        //            _controlsRenderPass->add(renderData, node->getTransform()->getModelMatrix());
        //        else
        //            _glassyControlsRenderPass->add(renderData, node->getTransform()->getModelMatrix());
        //    });
        //});

        auto pipeline = new phi::pipeline(_gl, (float)_width, (float)_height);

        auto reserveContainer = [&](GLenum internalFormat, size_t size)
        {
            auto layout = phi::textureContainerLayout();
            layout.w = static_cast<GLsizei>(_width);
            layout.h = static_cast<GLsizei>(_height);
            layout.levels = 1;
            layout.internalFormat = internalFormat;
            layout.wrapMode = GL_REPEAT;
            layout.minFilter = GL_NEAREST;
            layout.magFilter = GL_NEAREST;

            _gl->texturesManager->reserveContainer(layout, size);
        };

        auto createRenderTarget = [&](
            GLenum attachment,
            GLenum internalFormat,
            GLenum dataFormat)
        {
            auto texture = new phi::texture(static_cast<uint>(_width), static_cast<uint>(_height));
            texture->internalFormat = internalFormat;
            texture->dataFormat = dataFormat;
            texture->minFilter = GL_NEAREST;
            texture->magFilter = GL_NEAREST;
            texture->generateMipmaps = false;
            texture->data = 0;

            auto textureAddress = _gl->texturesManager->get(texture);
            return new phi::renderTarget(
                attachment,
                static_cast<GLint>(_width),
                static_cast<GLint>(_height),
                textureAddress,
                texture);
        };

        reserveContainer(GL_RGBA16F, 3);
        reserveContainer(GL_RGBA8, 1);
        reserveContainer(GL_DEPTH_COMPONENT32, 1);

        auto rt0 = createRenderTarget(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA);
        auto rt1 = createRenderTarget(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA);
        auto rt2 = createRenderTarget(GL_COLOR_ATTACHMENT2, GL_RGBA16F, GL_RGBA);
        auto rt3 = createRenderTarget(GL_COLOR_ATTACHMENT3, GL_RGBA8, GL_RGBA);
        auto depth = createRenderTarget(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT);

        auto gBufferFramebuffer = new framebuffer();
        gBufferFramebuffer->add(rt0);
        gBufferFramebuffer->add(rt1);
        gBufferFramebuffer->add(rt2);
        gBufferFramebuffer->add(rt3);
        gBufferFramebuffer->add(depth);

        auto rtAddresses = new phi::renderTargetsAddresses();
        rtAddresses->rt0Unit = rt0->textureAddress.unit;
        rtAddresses->rt1Unit = rt1->textureAddress.unit;
        rtAddresses->rt2Unit = rt2->textureAddress.unit;
        rtAddresses->rt3Unit = rt3->textureAddress.unit;
        rtAddresses->depthUnit = depth->textureAddress.unit;
        rtAddresses->rt0Page = rt0->textureAddress.page;
        rtAddresses->rt1Page = rt1->textureAddress.page;
        rtAddresses->rt2Page = rt2->textureAddress.page;
        rtAddresses->rt3Page = rt3->textureAddress.page;
        rtAddresses->depthPage = depth->textureAddress.page;

        auto finalImageTexture = new texture(
            static_cast<uint>(_width),
            static_cast<uint>(_height),
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr,
            GL_TEXTURE_2D,
            GL_RGBA8,
            GL_CLAMP_TO_EDGE,
            GL_LINEAR_MIPMAP_LINEAR,
            GL_LINEAR,
            true);

        auto finalImageTexAddress = _gl->texturesManager->get(finalImageTexture);

        auto finalImageRT = new renderTarget(
            GL_COLOR_ATTACHMENT0,
            static_cast<GLint>(_width),
            static_cast<GLint>(_height),
            finalImageTexAddress,
            finalImageTexture);

        auto finalImageFramebuffer = new framebuffer();
        finalImageFramebuffer->add(finalImageRT);

        vector<string> gBufferShaderAttribs;
        gBufferShaderAttribs.push_back("inPosition");
        gBufferShaderAttribs.push_back("inTexCoord");
        gBufferShaderAttribs.push_back("inNormal");
        gBufferShaderAttribs.push_back("inTangent");
        gBufferShaderAttribs.push_back("inMaterialId");
        gBufferShaderAttribs.push_back("inModelMatrix");

        auto gBufferShader = _gl->shadersManager->load("geometryPass", gBufferShaderAttribs);
        gBufferShader->addUniform(0, "textureArrays");

        auto gBuffer = new renderPass(gBufferShader);
        gBuffer->setOnUpdate([=](shader* shader)
        {
            shader->bind();

            if (_gl->currentState.useBindlessTextures)
                shader->setUniform(0, _gl->texturesManager->handles);
            else
                shader->setUniform(0, _gl->texturesManager->units);

            shader->unbind();
        });

        gBuffer->setOnRender([=](shader* shader)
        {
            gBufferFramebuffer->bindForDrawing();

            const auto selectionRenderTargetNumber = 3;
            auto selectionClearColor = 0.0f;

            glDepthMask(GL_TRUE);
            glError::check();
            glEnable(GL_DEPTH_TEST);
            glError::check();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glError::check();
            glClearBufferfv(GL_COLOR, selectionRenderTargetNumber, &selectionClearColor);
            glError::check();

            shader->bind();

            for (auto batch : pipeline->getBatches())
                batch->render();

            shader->unbind();

            gBufferFramebuffer->unbind(GL_FRAMEBUFFER);
            
            gBufferFramebuffer->bindForReading();
            auto w = static_cast<GLint>(_width);
            auto h = static_cast<GLint>(_height);

            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glError::check();

            finalImageFramebuffer->bindForDrawing();
        });
        //////////////////////////////////////////////////////

        GLuint quadVao;
        auto quad = geometry::createQuad(2.0f);

        glCreateVertexArrays(1, &quadVao);
        glError::check();

        glBindVertexArray(quadVao);
        glError::check();

        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        auto quadVbo = new vertexBuffer<vertex>(attribs);
        quadVbo->storage(quad->vboSize, quad->vboData, bufferStorageUsage::write);

        auto quadEbo = new buffer<uint>(bufferTarget::element);
        quadEbo->storage(quad->eboSize, quad->eboData, bufferStorageUsage::write);

        glBindVertexArray(0);
        glError::check();

        vector<string> lightingShaderAttribs;
        lightingShaderAttribs.push_back("inPosition");
        lightingShaderAttribs.push_back("inTexCoord");

        auto lightingShader = _gl->shadersManager->load("lightingPass", lightingShaderAttribs);
        lightingShader->addUniform(0, "textureArrays");

        auto rtsBuffer = new buffer<renderTargetsAddresses>(bufferTarget::uniform);

        rtsBuffer->storage(
            sizeof(renderTargetsAddresses),
            &*rtAddresses,
            bufferStorageUsage::write);

        rtsBuffer->bindBufferBase(2);

        auto lighting = new renderPass(lightingShader);

        lighting->setOnUpdate([=](shader* shader)
        {
            shader->bind();

            if (_gl->currentState.useBindlessTextures)
                shader->setUniform(0, _gl->texturesManager->handles);
            else
                shader->setUniform(0, _gl->texturesManager->units);

            shader->unbind();
        });

        lighting->setOnRender([=](shader* shader)
        {
            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

            glBindVertexArray(quadVao);
            glError::check();

            shader->bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glError::check();
            shader->unbind();

            glBindVertexArray(0);
            glError::check();

            glEnable(GL_DEPTH_TEST);

            finalImageFramebuffer->blitToDefault(finalImageRT);
        });

        //auto selection = new renderPass();
        //auto planeGrid = new renderPass();

        //_finalImageFramebuffer->blitToDefault(_finalImageRT);
        //generateFinalImageMipMaps();

        auto sceneLayer = new layer({ gBuffer, lighting});
        sceneLayer->setOnNodeAdded([=](node* addedChild)
        {
            addedChild->traverse([=](phi::node* node)
            {
                auto mesh = node->getComponent<phi::mesh>();
                if (mesh)
                {
                    sceneId::setNextId(mesh);
                    pipeline->add(mesh, node->getTransform()->getModelMatrix());
                }
            });
        });

        auto floor = _userLibrary->getObjectsRepository()->getAllResources()[2]->getClonedObject();
        sceneLayer->add(floor);

        auto chair = _userLibrary->getObjectsRepository()->getAllResources()[0]->getClonedObject();
        chair->getTransform()->setLocalPosition(vec3(0.f, .1f, .0f));
        sceneLayer->add(chair);

        vector<layer*> layers = { sceneLayer };
        _designContext = new context(layers);



        //nandinhoLayer->add(labelNandinho);
        //nandinhoLayer->add(_labelFps);
    }

    void screen::initInput()
    {
        _commandsManager = new commandsManager();
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_z }, [&]() { return new undoCommand(_commandsManager); }));
        _commandsManager->addShortcut(shortcut({ PHIK_CTRL, PHIK_y }, [&]() { return new redoCommand(_commandsManager); }));
        _commandsManager->addShortcut(shortcut({ PHIK_DELETE }, [&]()
        {
            return new multiCommand(vector<command*>
            {
                new unselectSceneObjectCommand(_scene->getSelectedObjects()),
                new deleteSceneObjectCommand(_scene->getSelectedObjects())
            });
        }));

        _defaultController = new defaultCameraController(_scene, _commandsManager);
    }

    void screen::onUpdate()
    {
        phi::floatAnimator::update();
        _defaultController->update();
        
        _designContext->update();

        _scene->update();
        //_ui->update();
    }

    void screen::onRender()
    {
        //_scene->render();
        //_ui->render();

        _designContext->render();
    }

    void screen::onTick()
    {
        auto label = _labelFps->getComponent<phi::text>();
        auto str = "fps: " + std::to_string(application::framesPerSecond);
        debug(str);
        label->setText(wstring(str.begin(), str.end()));

#ifdef _DEBUG
        while (!_messageQueue->empty())
        {
            auto message = _messageQueue->front();
            message.callback(message.fileChanged);
            _messageQueue->pop();
        }
#endif
    }

    void screen::onClosing()
    {
        safeDelete(_commandsManager);
        safeDelete(_defaultController);
        safeDelete(_gl);
        safeDelete(_userLibrary);
        safeDelete(_projectLibrary);
        safeDelete(_scene);
        safeDelete(_ui);
#ifdef _DEBUG
        _watcher->endWatch();
        safeDelete(_watcher);
        safeDelete(_messageQueue);
#endif 
    }
}