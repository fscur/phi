#include <precompiled.h>
#include "horizontalBlurRenderPass.h"

namespace phi
{
    horizontalBlurRenderPass::horizontalBlurRenderPass(renderTarget* source, gl* gl, float w, float h) :
        _gl(gl),
        _quad(nullptr),
        _shader(nullptr),
        _quadVbo(nullptr),
        _quadEbo(nullptr),
        _w(w),
        _h(h),
        _quadVao(0u),
        _resolution(w, h),
        _framebuffer(new framebuffer()),
        _source(source),
        _result(nullptr)
    {
        createQuad();
        initShader();
        initFramebuffer();
    }

    horizontalBlurRenderPass::~horizontalBlurRenderPass()
    {
        safeDelete(_quad);
        safeDelete(_quadVbo);
        safeDelete(_quadEbo);
    }

    void horizontalBlurRenderPass::createQuad()
    {
        _quad = geometry::createQuad(2.0f);

        glCreateVertexArrays(1, &_quadVao);
        glError::check();

        glBindVertexArray(_quadVao);
        glError::check();

        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        _quadVbo = new vertexBuffer(attribs);
        _quadVbo->storage(_quad->vboSize, _quad->vboData, bufferStorageUsage::write);

        _quadEbo = new buffer(bufferTarget::element);
        _quadEbo->storage(_quad->eboSize, _quad->eboData, bufferStorageUsage::write);

        glBindVertexArray(0);
        glError::check();
    }

    void horizontalBlurRenderPass::initShader()
    {
        vector<string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        _shader = _gl->shadersManager->load("blurH", attribs);

        _shader->addUniform(0, "textureArrays");
        _shader->addUniform(1, "unit");
        _shader->addUniform(2, "page");
        _shader->addUniform(3, "resolution");
        _shader->addUniform(4, "level");
    }

    void horizontalBlurRenderPass::initFramebuffer()
    {
        /*auto reserveContainer = [&](GLenum internalFormat, size_t size)
        {
            auto layout = phi::textureContainerLayout();
            layout.w = static_cast<GLsizei>(_w);
            layout.h = static_cast<GLsizei>(_h);
            layout.levels = 1;
            layout.internalFormat = internalFormat;
            layout.wrapMode = GL_CLAMP_TO_EDGE;
            layout.minFilter = GL_LINEAR;
            layout.magFilter = GL_LINEAR;

            _gl->texturesManager->reserveContainer(layout, size);
        };*/

        auto texture = new phi::texture(static_cast<uint>(_w), static_cast<uint>(_h));
        texture->internalFormat = GL_RGBA8;
        texture->dataFormat = GL_RGBA;
        texture->dataType = GL_UNSIGNED_BYTE;
        texture->wrapMode = GL_CLAMP_TO_EDGE;
        texture->minFilter = GL_LINEAR;
        texture->magFilter = GL_LINEAR;
        texture->generateMipmaps = false;

        auto textureAddress = _gl->texturesManager->add(texture);

        _result = new phi::renderTarget(
            GL_COLOR_ATTACHMENT0,
            static_cast<GLint>(_w),
            static_cast<GLint>(_h),
            textureAddress,
            texture);

        _framebuffer->add(_result);
    }

    void horizontalBlurRenderPass::update()
    {
    }

    void horizontalBlurRenderPass::render()
    {
        glDisable(GL_DEPTH_TEST);
        glError::check();

        _framebuffer->bindForDrawing();

        glError::check();
        glClear(GL_COLOR_BUFFER_BIT);

        _shader->bind();
        _shader->setUniform(0, _gl->texturesManager->units);
        _shader->setUniform(1, _source->textureAddress.unit);
        _shader->setUniform(2, _source->textureAddress.page);
        _shader->setUniform(3, _resolution);
        _shader->setUniform(4, 4);

        glBindVertexArray(_quadVao);
        glError::check();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glError::check();

        _shader->unbind();

        glBindVertexArray(0);
        glError::check();

        _framebuffer->unbind(GL_FRAMEBUFFER);

        glEnable(GL_DEPTH_TEST);
    }
}