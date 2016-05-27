#include <precompiled.h>
#include "layerBuilder.h"

#include <rendering\pipeline.h>
#include <rendering\frameBuffer.h>
#include <rendering\texture.h>
#include <rendering\camera.h>

#include <scenes\sceneId.h>

#include <ui\control.h>
#include <ui\controlRenderData.h>
#include <ui\text.h>
#include <ui\textRenderPass.h>

namespace demon
{
    using namespace phi;

    layer* layerBuilder::buildScene(gl* gl, float width, float height)
    {
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

        auto gBuffer = new renderPass(gl->shadersManager->loadCrazyFuckerSpecificShader("geometryPass"));
        gBuffer->setOnUpdate([=](shader* shader)
        {
            shader->bind();

            if (gl->currentState.useBindlessTextures)
                shader->setUniform(0, gl->texturesManager->handles);
            else
                shader->setUniform(0, gl->texturesManager->units);

            shader->unbind();
        });

        gBuffer->setOnRender([=](shader* shader)
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

            shader->bind();

            for (auto batch : pipeline->getBatches())
                batch->render();

            shader->unbind();

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

        auto lighting = new renderPass(gl->shadersManager->loadCrazyFuckerSpecificShader("lightingPass"));

        lighting->setOnUpdate([=](shader* shader)
        {
            shader->bind();

            if (gl->currentState.useBindlessTextures)
                shader->setUniform(0, gl->texturesManager->handles);
            else
                shader->setUniform(0, gl->texturesManager->units);

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

        auto sceneLayer = new layer({ gBuffer, lighting });
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

    layer * layerBuilder::buildUI(gl* gl, float width, float height)
    {
        /////////////// BUFFERS ///////////////

        auto uiCamera = new camera("uiCamera", width, height, 0.1f, 10000.0f, PI_OVER_4);

        auto cameraNode = new node("uiCamera");
        cameraNode->addComponent(uiCamera);

        uiCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
        uiCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

        auto frameUniformBlockBuffer = new buffer<frameUniformBlock>(bufferTarget::uniform);

        auto frameUniform = frameUniformBlock();
        frameUniform.p = uiCamera->getProjectionMatrix();
        frameUniform.v = uiCamera->getViewMatrix();
        frameUniform.vp = frameUniform.p * frameUniform.v;
        frameUniform.ip = glm::inverse(frameUniform.p);

        frameUniformBlockBuffer->storage(
            sizeof(phi::frameUniformBlock),
            &frameUniform,
            bufferStorageUsage::dynamic | bufferStorageUsage::write);

        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 1.0f))
        };
        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };

        auto textVertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 1.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 0.0f))
        };
        auto textIndices = vector<uint>{ 0, 1, 2, 2, 3, 0 };
        auto controlQuad = geometry::create(vertices, indices);

        GLuint controlVao;
        glCreateVertexArrays(1, &controlVao);
        glBindVertexArray(controlVao);

        vector<vertexAttrib> controlsVboAttribs;
        controlsVboAttribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        controlsVboAttribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        auto controlsVbo = new vertexBuffer<vertex>(controlsVboAttribs);
        controlsVbo->storage(controlQuad->vboSize, controlQuad->vboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        auto controlsEbo = new buffer<uint>(bufferTarget::element);
        controlsEbo->storage(controlQuad->eboSize, controlQuad->eboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        vector<vertexAttrib> controlsModelMatricesAttribs;
        for (uint i = 0; i < 4; ++i)
            controlsModelMatricesAttribs.push_back(vertexAttrib(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));
        auto controlsModelMatricesBuffer = new vertexBuffer<mat4>(controlsModelMatricesAttribs);
        controlsModelMatricesBuffer->data(sizeof(mat4), nullptr, bufferDataUsage::dynamicDraw);

        glBindVertexArray(0);

        auto controlsRenderDataBuffer = new buffer<controlRenderData>(bufferTarget::shader);
        controlsRenderDataBuffer->data(sizeof(controlRenderData), nullptr, bufferDataUsage::dynamicDraw);
        controlsRenderDataBuffer->bindBufferBase(1);

        /////////////// BUFFERS ///////////////

        auto controlsRenderData = new vector<controlRenderData>();
        auto controlsModelMatrices = new vector<mat4>();
        auto controls = new vector<control*>();

        auto controlRenderPass = new renderPass(gl->shadersManager->loadCrazyFuckerSpecificShader("control"));

        controlRenderPass->setOnUpdate([=](phi::shader* shader)
        {
        });

        controlRenderPass->setOnRender([=](phi::shader* shader)
        {
            frameUniformBlockBuffer->bindBufferBase(0);
            controlsRenderDataBuffer->bindBufferBase(1);

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendColor(1, 1, 1, 1);

            shader->bind();
            shader->setUniform(0, gl->texturesManager->units);

            glBindVertexArray(controlVao);
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, (GLsizei)controls->size());
            glBindVertexArray(0);

            shader->unbind();

            glBlendColor(0, 0, 0, 0);
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        });

        /////////////// TEXT BUFFER ///////////////

        GLuint textVao;
        glCreateVertexArrays(1, &textVao);
        glBindVertexArray(textVao);

        auto textQuad = geometry::create(textVertices, textIndices);

        vector<vertexAttrib> textVboAttribs;
        textVboAttribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        textVboAttribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        textVboAttribs.push_back(vertexAttrib(2, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::normal)));
        textVboAttribs.push_back(vertexAttrib(3, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::tangent)));

        auto textVbo = new vertexBuffer<vertex>(textVboAttribs);
        textVbo->storage(textQuad->vboSize, textQuad->vboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        auto textEbo = new buffer<uint>(bufferTarget::element);
        textEbo->storage(textQuad->eboSize, textQuad->eboData, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        vector<vertexAttrib> glyphsIdsAttribs;
        glyphsIdsAttribs.push_back(vertexAttrib(4, 1, GL_UNSIGNED_INT, 0, 0, 1));
        auto glyphIdsBuffer = new vertexBuffer<uint>(glyphsIdsAttribs);
        glyphIdsBuffer->data(sizeof(uint), nullptr, bufferDataUsage::dynamicDraw);

        vector<vertexAttrib> textModelMatricesAttribs;

        for (uint i = 0; i < 4; ++i)
            textModelMatricesAttribs.push_back(vertexAttrib(5 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        auto textModelMatricesBuffer = new vertexBuffer<mat4>(textModelMatricesAttribs);
        textModelMatricesBuffer->data(sizeof(mat4), nullptr, bufferDataUsage::dynamicDraw);

        auto glyphInfoBuffer = new buffer<glyphInfo>(bufferTarget::shader);
        glyphInfoBuffer->data(sizeof(glyphInfo), nullptr, bufferDataUsage::dynamicDraw);

        glBindVertexArray(0);

        /////////////// TEXT BUFFER ///////////////

        auto textModelMatrices = new vector<mat4>();
        auto glyphInfos = new vector<glyphInfo>();

        auto textRenderPass = new renderPass(gl->shadersManager->loadCrazyFuckerSpecificShader("text"));
        textRenderPass->setOnUpdate([=](shader* shader) {});

        textRenderPass->setOnRender([=](shader* shader)
        {
            frameUniformBlockBuffer->bindBufferBase(0);
            glyphInfoBuffer->bindBufferBase(1);

            auto texelSize = 1.0f / (float)gl->fontsManager->getGlyphAtlasSize();

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendColor(1, 1, 1, 1);

            shader->bind();
            shader->setUniform(0, gl->texturesManager->units);
            shader->setUniform(1, glm::vec2(texelSize, texelSize));

            glBindVertexArray(textVao);
            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(glyphInfos->size()));
            glBindVertexArray(0);

            shader->unbind();

            glBlendColor(0, 0, 0, 0);
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);

            glEnable(GL_CULL_FACE);
        });

        auto uiLayer = new layer({controlRenderPass, textRenderPass});

        auto labels = new vector<text*>();

        uiLayer->setOnNodeAdded([=](node* node)
        {
            node->traverse([=](phi::node* currentNode)
            {
                auto control = currentNode->getComponent<phi::control>();
                auto text = currentNode->getComponent<phi::text>();
                if (control)
                {
                    controls->push_back(control);

                    phi::texture* texture = pipeline::getTextureFromImage(control->getBackgroundImage(), gl->defaultAlbedoImage);
                    auto address = gl->texturesManager->get(texture);

                    auto renderData = controlRenderData();
                    renderData.backgroundColor = control->getBackgroundColor();
                    renderData.backgroundTextureUnit = address.unit;
                    renderData.backgroundTexturePage = address.page;

                    controlsRenderData->push_back(renderData);
                    controlsModelMatrices->push_back(currentNode->getTransform()->getModelMatrix());

                    controlsModelMatricesBuffer->data(sizeof(mat4) * controls->size(), &controlsModelMatrices->at(0), bufferDataUsage::dynamicDraw);
                    controlsRenderDataBuffer->data(sizeof(controlRenderData) * controls->size(), &controlsRenderData->at(0), bufferDataUsage::dynamicDraw);
                }
                if (text)
                {
                    labels->push_back(text);

                    auto renderData = textRenderData();
                    renderData.text = text->getText();
                    renderData.position = node->getTransform()->getPosition();
                    renderData.font = text->getFont();
                    renderData.color = text->getColor();

                    auto font = renderData.font;

                    if (font == nullptr)
                        font = gl->defaultFont;

                    float baseLine = font->getBaseLine();
                    float spacing = font->getSpacing();
                    float x = renderData.position.x + spacing;
                    float y = renderData.position.y - baseLine + spacing;
                    float z = renderData.position.z;

                    glyph* previousGlyph = nullptr;
                    size_t textLength = renderData.text.length();

                    auto glyph = gl->fontsManager->getGlyph(font, (ulong)renderData.text[0]);
                    x -= glyph->offsetX;

                    for (size_t i = 0; i < textLength; i++)
                    {
                        glyph = gl->fontsManager->getGlyph(font, (ulong)renderData.text[i]);
                        auto kern = font->getKerning(previousGlyph, glyph);
                        auto w = glyph->width;
                        auto h = glyph->height;
                        auto x0 = x + glyph->offsetX;
                        auto y0 = y - h + glyph->offsetY;

                        auto modelMatrix = mat4(
                            w, 0.0f, 0.0f, 0.0f,
                            0.0f, h, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            x0, y0, z, 1.0f);

                        textModelMatrices->push_back(modelMatrix);

                        x += glyph->horiAdvance + kern.x;

                        float shift = std::abs(x0 - static_cast<int>(x0));

                        glyphInfo info;
                        info.pos = glyph->texPos;
                        info.size = glyph->texSize;
                        info.shift = shift;
                        info.unit = glyph->texUnit;
                        info.page = glyph->texPage;
                        info.color = renderData.color;

                        glyphInfos->push_back(info);

                        previousGlyph = glyph;
                    }

                    size_t glyphCount = glyphInfos->size();
                    vector<uint> glyphIds;

                    for (uint i = 0u; i < glyphCount; i++)
                        glyphIds.push_back(i);

                    glyphIdsBuffer->data(sizeof(uint) * glyphCount, &glyphIds[0], bufferDataUsage::dynamicDraw);
                    textModelMatricesBuffer->data(sizeof(mat4) * textModelMatrices->size(), &textModelMatrices->at(0), bufferDataUsage::dynamicDraw);
                    glyphInfoBuffer->data(sizeof(glyphInfo) * glyphCount, &glyphInfos->at(0), bufferDataUsage::dynamicDraw);
                }
            });
        });

        return uiLayer;
    }
}