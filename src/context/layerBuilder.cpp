#include <precompiled.h>
#include "layerBuilder.h"

#include <rendering\frameBuffer.h>
#include <rendering\texture.h>
#include <rendering\camera.h>
#include <rendering\glslCompiler.h>
#include <rendering\renderTargetsAddresses.h>
#include <rendering\vertexBuffer.h>

#include <sceneRendering\pipeline.h>

#include <uiRendering\glyphRenderData.h>
#include <uiRendering\controlRenderer.h>
#include <uiRendering\controlRenderData.h>
#include <uiRendering\textRenderer.h>
#include <uiRendering\textRenderData.h>

#include <context\sceneId.h>

#include <ui\control.h>
#include <ui\text.h>

#include <io\path.h>

namespace demon
{
    using namespace phi;

    program* layerBuilder::buildControlRenderPassProgram(const string& shadersPath)
    {
        auto vertFile = path::combine(shadersPath, "control", shader::VERT_EXT);
        auto fragFile = path::combine(shadersPath, "control", shader::FRAG_EXT);

        auto vertexShader = new phi::shader(vertFile);
        auto fragmentShader = new phi::shader(fragFile);
        auto program = glslCompiler::compile({ vertexShader, fragmentShader });

        return program;
    }

    program* layerBuilder::buildTextRenderPassProgram(const string& shadersPath)
    {
        auto vertFile = path::combine(shadersPath, "text", shader::VERT_EXT);
        auto fragFile = path::combine(shadersPath, "text", shader::FRAG_EXT);

        auto vertexShader = new phi::shader(vertFile);
        auto fragmentShader = new phi::shader(fragFile);
        auto program = glslCompiler::compile({vertexShader, fragmentShader});

        return program;
    }

    program* layerBuilder::buildGBufferRenderPassProgram(const string& shadersPath)
    {
        auto vertFile = path::combine(shadersPath, "gBuffer", shader::VERT_EXT);
        auto fragFile = path::combine(shadersPath, "gBuffer", shader::FRAG_EXT);

        auto vertexShader = new phi::shader(vertFile);
        auto fragmentShader = new phi::shader(fragFile);
        auto program = glslCompiler::compile({ vertexShader, fragmentShader });
        
        return program;
    }

    program* layerBuilder::buildLightingRenderPassProgram(const string& shadersPath)
    {
        auto vertFile = path::combine(shadersPath, "lighting", shader::VERT_EXT);
        auto fragFile = path::combine(shadersPath, "lighting", shader::FRAG_EXT);

        auto vertexShader = new phi::shader(vertFile);
        auto fragmentShader = new phi::shader(fragFile);
        auto program = glslCompiler::compile({ vertexShader, fragmentShader });

        return program;
    }

    layer* layerBuilder::buildScene(const string& resourcesPath, gl* gl, float width, float height)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto pipeline = new phi::pipeline(gl, width, height);

        auto reserveContainer = [&](GLenum internalFormat, size_t size)
        {
            auto layout = phi::textureContainerLayout();
            layout.w = static_cast<GLsizei>(width);
            layout.h = static_cast<GLsizei>(height);
            layout.levels = 1;
            layout.internalFormat = internalFormat;
            layout.wrapMode = GL_REPEAT;
            layout.minFilter = GL_NEAREST;
            layout.magFilter = GL_NEAREST;

            gl->texturesManager->reserveContainer(layout, size);
        };

        auto createRenderTarget = [&](
            GLenum attachment,
            GLenum internalFormat,
            GLenum dataFormat)
        {
            auto texture = new phi::texture(static_cast<uint>(width), static_cast<uint>(height));
            texture->internalFormat = internalFormat;
            texture->dataFormat = dataFormat;
            texture->minFilter = GL_NEAREST;
            texture->magFilter = GL_NEAREST;
            texture->generateMipmaps = false;
            texture->data = 0;

            auto textureAddress = gl->texturesManager->get(texture);
            return new phi::renderTarget(
                attachment,
                static_cast<GLint>(width),
                static_cast<GLint>(height),
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
            static_cast<uint>(width),
            static_cast<uint>(height),
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr,
            GL_TEXTURE_2D,
            GL_RGBA8,
            GL_CLAMP_TO_EDGE,
            GL_LINEAR_MIPMAP_LINEAR,
            GL_LINEAR,
            true);

        auto finalImageTexAddress = gl->texturesManager->get(finalImageTexture);

        auto finalImageRT = new renderTarget(
            GL_COLOR_ATTACHMENT0,
            static_cast<GLint>(width),
            static_cast<GLint>(height),
            finalImageTexAddress,
            finalImageTexture);

        auto finalImageFramebuffer = new framebuffer();
        finalImageFramebuffer->add(finalImageRT);
        
        auto gBufferRenderPassProgram = buildGBufferRenderPassProgram(shadersPath);
        auto gBuffer = new renderPass(gBufferRenderPassProgram);
        
        gBuffer->setOnUpdate([=](program* program)
        {
            program->bind();

            if (gl->currentState.useBindlessTextures)
                program->setUniform(0, gl->texturesManager->handles);
            else
                program->setUniform(0, gl->texturesManager->units);

            program->unbind();
        });

        gBuffer->setOnRender([=](program* program)
        {
            gBufferFramebuffer->bindForDrawing();
            pipeline->bindMaterialsIdsBuffer();

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

            program->bind();

            for (auto batch : pipeline->getBatches())
                batch->render();

            program->unbind();

            gBufferFramebuffer->unbind(GL_FRAMEBUFFER);

            gBufferFramebuffer->bindForReading();
            auto w = static_cast<GLint>(width);
            auto h = static_cast<GLint>(height);

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

        auto rtsBuffer = new buffer<renderTargetsAddresses>(bufferTarget::uniform);

        rtsBuffer->storage(
            sizeof(renderTargetsAddresses),
            &*rtAddresses,
            bufferStorageUsage::write);
        rtsBuffer->bindBufferBase(2);

        auto lightingRenderPassProgram = buildLightingRenderPassProgram(shadersPath);
        auto lighting = new renderPass(lightingRenderPassProgram);

        lighting->setOnUpdate([=](program* program)
        {
            program->bind();

            if (gl->currentState.useBindlessTextures)
                program->setUniform(0, gl->texturesManager->handles);
            else
                program->setUniform(0, gl->texturesManager->units);

            program->unbind();
        });

        lighting->setOnRender([=](program* program)
        {
            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

            glBindVertexArray(quadVao);
            glError::check();

            program->bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glError::check();
            program->unbind();

            glBindVertexArray(0);
            glError::check();

            glEnable(GL_DEPTH_TEST);

            finalImageFramebuffer->blitToDefault(finalImageRT);
        });

        //auto selection = new renderPass();
        //auto planeGrid = new renderPass();

        //_finalImageFramebuffer->blitToDefault(_finalImageRT);
        //generateFinalImageMipMaps();

        auto sceneCamera = new camera("sceneCamera", width, height, 0.1f, 10000.0f, PI_OVER_4);
        auto sceneLayer = new layer(sceneCamera, { gBuffer, lighting });
        auto cameraPosition = vec3(0.0f, 0.0f, 10.0f);
        sceneCamera->getTransform()->setLocalPosition(cameraPosition);
        sceneCamera->getTransform()->setDirection(-cameraPosition);

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

        return sceneLayer;
    }

    layer* layerBuilder::buildUI(const string& resourcesPath, gl* gl, float width, float height)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");
        auto controlRenderer = new phi::controlRenderer(gl);
        auto textRenderer = new phi::textRenderer(gl);

        auto controlRenderPassProgram = buildControlRenderPassProgram(shadersPath);
        auto controlRenderPass = new renderPass(controlRenderPassProgram);
        controlRenderPass->setOnUpdate([=](phi::program* program) {});
        controlRenderPass->setOnRender([=](phi::program* program)
        {
            controlRenderer->render(program);
        });

        auto textRenderPassProgram = buildTextRenderPassProgram(shadersPath);
        auto textRenderPass = new renderPass(textRenderPassProgram);
        textRenderPass->setOnUpdate([=](phi::program* program) {});
        textRenderPass->setOnRender([=](phi::program* program)
        {
            textRenderer->render(program);
        });

        auto uiCamera = new camera("uiCamera", width, height, 0.1f, 10000.0f, PI_OVER_4);
        auto uiLayer = new layer(uiCamera, { controlRenderPass, textRenderPass });
        uiCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
        uiCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

        uiLayer->setOnNodeAdded([=](node* node)
        {
            node->traverse([=](phi::node* currentNode)
            {
                auto control = currentNode->getComponent<phi::control>();
                auto text = currentNode->getComponent<phi::text>();
                if (control)
                {
                    controlRenderer->add(control);
                }
                if (text)
                {
                    auto font = text->getFont();

                    auto renderData = new textRenderData();
                    renderData->text = text->getText();
                    renderData->position = currentNode->getTransform()->getPosition();
                    renderData->font = font;
                    renderData->color = text->getColor();

                    textRenderer->add(renderData);
                }
            });
        });

        return uiLayer;
    }
}