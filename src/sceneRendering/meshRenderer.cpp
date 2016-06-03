#include <precompiled.h>
#include "meshRenderer.h"

#include <core\notImplementedException.h>
#include <core\node.h>

#include <io\path.h>

#include <rendering\glslCompiler.h>
#include <rendering\framebufferBuilder.h>

namespace phi
{
    meshRenderer::meshRenderer(gl* gl, float width, float height, vector<renderPass*>&& renderPasses) :
        _gl(gl),
        _width(width),
        _height(height),
        _renderPasses(renderPasses)
    {
    }

    meshRenderer::~meshRenderer()
    {
    }

    void meshRenderer::render()
    {
        for (auto renderPass : _renderPasses)
            renderPass->render();
    }

    meshRenderer* meshRenderer::configure(gl * gl, float width, float height, const string& resourcesPath, meshRendererDescriptor* rendererDescriptor)
    {
        auto getProgram = [](string vertexShaderName, string fragmentShaderName, string shadersPath)
        {
            auto vertexShaderFileName = path::combine(shadersPath, vertexShaderName, shader::VERT_EXT);
            auto fragmentShaderFileName = path::combine(shadersPath, fragmentShaderName, shader::FRAG_EXT);

            auto vertexShader = new phi::shader(vertexShaderFileName);
            auto fragmentShader = new phi::shader(fragmentShaderFileName);

            return glslCompiler::compile({ vertexShader, fragmentShader });
        };

        auto gBufferFramebuffer = framebufferBuilder::newFramebuffer(gl, width, height)
            ->with(GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_RGBA)
            ->with(GL_COLOR_ATTACHMENT1, GL_RGBA16F, GL_RGBA)
            ->with(GL_COLOR_ATTACHMENT2, GL_RGBA16F, GL_RGBA)
            ->with(GL_COLOR_ATTACHMENT3, GL_RGBA8, GL_RGBA)
            ->with(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT)
            ->build();

        auto renderTargets = gBufferFramebuffer->getRenderTargets();

        auto rtAddresses = new phi::renderTargetsAddresses();
        rtAddresses->rt0Unit = renderTargets[0]->textureAddress.unit;
        rtAddresses->rt1Unit = renderTargets[1]->textureAddress.unit;
        rtAddresses->rt2Unit = renderTargets[2]->textureAddress.unit;
        rtAddresses->rt3Unit = renderTargets[3]->textureAddress.unit;
        rtAddresses->depthUnit = renderTargets[4]->textureAddress.unit;
        rtAddresses->rt0Page = renderTargets[0]->textureAddress.page;
        rtAddresses->rt1Page = renderTargets[1]->textureAddress.page;
        rtAddresses->rt2Page = renderTargets[2]->textureAddress.page;
        rtAddresses->rt3Page = renderTargets[3]->textureAddress.page;
        rtAddresses->depthPage = renderTargets[4]->textureAddress.page;

        auto layout = textureLayout();
        layout.dataFormat = GL_RGBA;
        layout.dataType = GL_UNSIGNED_BYTE;
        layout.internalFormat = GL_RGBA8;
        layout.wrapMode = GL_CLAMP_TO_EDGE;
        layout.minFilter = GL_LINEAR_MIPMAP_LINEAR;
        layout.magFilter = GL_LINEAR;

        auto finalImageTexture = new phi::texture(
            static_cast<uint>(width),
            static_cast<uint>(height),
            layout,
            nullptr,
            true,
            false);

        auto finalImageTexAddress = gl->texturesManager->get(finalImageTexture);

        auto finalImageRT = new renderTarget(
            GL_COLOR_ATTACHMENT0,
            static_cast<GLint>(width),
            static_cast<GLint>(height),
            finalImageTexAddress,
            finalImageTexture);

        auto finalImageFramebuffer = new framebuffer();
        finalImageFramebuffer->add(finalImageRT);
        
        auto quad = geometry::createQuad(2.0f);

        GLuint quadVao;
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

        auto shadersPath = path::combine(resourcesPath, "shaders");

        auto gBuffer = new renderPass(getProgram("gBuffer", "gBuffer", shadersPath));
        gBuffer->setOnRender([=](program* program)
        {
            rendererDescriptor->bind();
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

            program->bind();

            if (gl->currentState.useBindlessTextures)
                program->setUniform(0, gl->texturesManager->handles);
            else
                program->setUniform(0, gl->texturesManager->units);

            for (auto batch : rendererDescriptor->_batches)
                batch->render();

            program->unbind();

            gBufferFramebuffer->unbind(GL_FRAMEBUFFER);

            gBufferFramebuffer->bindForReading();
            auto w = static_cast<GLint>(width);
            auto h = static_cast<GLint>(height);

            glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glError::check();
        });

        auto lighting = new renderPass(getProgram("lighting", "lighting", shadersPath));
        lighting->setOnRender([=](program* program)
        {
            finalImageFramebuffer->bindForDrawing();

            glDisable(GL_DEPTH_TEST);
            glClear(GL_COLOR_BUFFER_BIT);

            glBindVertexArray(quadVao);
            glError::check();

            program->bind();

            if (gl->currentState.useBindlessTextures)
                program->setUniform(0, gl->texturesManager->handles);
            else
                program->setUniform(0, gl->texturesManager->units);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glError::check();
            program->unbind();

            glBindVertexArray(0);
            glError::check();

            glEnable(GL_DEPTH_TEST);

            finalImageFramebuffer->blitToDefault(finalImageRT);
        });

        return new meshRenderer(gl, width, height, { gBuffer, lighting });
    }
}