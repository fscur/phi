#include <precompiled.h>
#include "textRenderPass.h"

namespace phi
{
    inline textRenderPass::textRenderPass(const gl* const gl, float w, float h) :
        _gl(gl),
        _w(w),
        _h(h)
    {
        _fontManager = new fontManager(_gl->texturesManager);

        initShader();
        initCamera();
        createQuad();
        createVao();
    }

    inline textRenderPass::~textRenderPass()
    {
        safeDelete(_quad);
        safeDelete(_shader);
        safeDelete(_fontManager);
        safeDelete(_vbo);
        safeDelete(_modelMatricesBuffer);
        safeDelete(_glyphIdsBuffer);
        safeDelete(_ebo);
        safeDelete(_glyphInfoBuffer);
    }

    void textRenderPass::initShader()
    {
        vector<string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");
        attribs.push_back("inGlyphId");
        attribs.push_back("inModelMatrix");

        _shader = _gl->shadersManager->load("text", attribs);
        _shader->addUniform(0, "v");
        _shader->addUniform(1, "p");
        _shader->addUniform(2, "glyphAtlas");
        _shader->addUniform(3, "texelSize");
    }

    void textRenderPass::initCamera()
    {
        auto size = 1.0f;
        auto hw = (float)_w * 0.5f;
        auto hh = (float)_h * 0.5f;

        _projectionMatrix = glm::ortho<float>(-hw, hw, -hh, hh, 0, 1000);

        /*_viewMatrix = glm::lookAt<float>(
            vec3(hw, -hh, 1.0f),
            vec3(hw, -hh, -1.0f),
            vec3(0.0f, 1.0f, 0.0f));*/

        _viewMatrix = glm::lookAt<float>(
            vec3(0.0f, 0.0f, 1.0f),
            vec3(0.0f, 0.0f, -1.0f),
            vec3(0.0f, 1.0f, 0.0f));
    }

    void textRenderPass::createQuad()
    {
        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 1.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 0.0f))
        };

        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };

        _quad = geometry::create(vertices, indices);
    }

    void textRenderPass::createVao()
    {
        glCreateVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        auto geometry = _quad;
        createVbo(geometry->vboData, geometry->vboSize);
        createEbo(geometry->eboData, geometry->eboSize);
        createGlyphIdsBuffer();
        createModelMatricesBuffer();
        createGlyphInfoBuffer();

        glBindVertexArray(0);
    }

    void textRenderPass::createVbo(void* const data, GLsizeiptr size)
    {
        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        attribs.push_back(vertexAttrib(2, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::normal)));
        attribs.push_back(vertexAttrib(3, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::tangent)));

        _vbo = new vertexBuffer(attribs);
        _vbo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void textRenderPass::createGlyphIdsBuffer()
    {
        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(4, 1, GL_UNSIGNED_INT, 0, 0, 1));

        _glyphIdsBuffer = new vertexBuffer(attribs);
        _glyphIdsBuffer->data(sizeof(uint), nullptr, bufferDataUsage::dynamicDraw);
    }

    void textRenderPass::createModelMatricesBuffer()
    {
        vector<vertexAttrib> attribs;

        for (uint i = 0; i < 4; ++i)
            attribs.push_back(vertexAttrib(5 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new vertexBuffer(attribs);
        _modelMatricesBuffer->data(sizeof(mat4), nullptr, bufferDataUsage::dynamicDraw);
    }

    void textRenderPass::createEbo(void* const data, GLsizeiptr size)
    {
        _ebo = new buffer(bufferTarget::element);
        _ebo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void textRenderPass::createGlyphInfoBuffer()
    {
        _glyphInfoBuffer = new buffer(bufferTarget::shader);
        _glyphInfoBuffer->data(sizeof(glyphInfo), nullptr, bufferDataUsage::dynamicDraw);
    }

    void textRenderPass::updateBuffers()
    {
        size_t glyphCount = _glyphInfos.size();
        vector<uint> glyphIds;

        for (uint i = 0u; i < glyphCount; i++)
            glyphIds.push_back(i);

        _glyphIdsBuffer->data(sizeof(uint) * glyphCount, &glyphIds[0], bufferDataUsage::dynamicDraw);

        _modelMatricesBuffer->data(sizeof(mat4) * _modelMatrices.size(), &_modelMatrices[0], bufferDataUsage::dynamicDraw);

        _glyphInfoBuffer->data(sizeof(glyphInfo) * glyphCount, &_glyphInfos[0], bufferDataUsage::dynamicDraw);
    }

    void textRenderPass::addText(wstring text, vec3 position, font* const font)
    {
        float lineHeight = font->getLineHeight();

        float x = position.x;
        float y = position.y + lineHeight;
        float z = position.z;

        glyph* previousGlyph = nullptr;
        size_t textLength = text.length();

        for (size_t i = 0; i < textLength; i++)
        {
            auto glyph = _fontManager->getGlyph(font, (ulong)text[i]);
            auto kern = font->getKerning(previousGlyph, glyph);
            auto w = glyph->width;
            auto h = glyph->height;
            auto x0 = x + glyph->offsetX;

            auto modelMatrix = mat4(
                w, 0.0f, 0.0f, 0.0f,
                0.0f, h, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                x0, -y - h + glyph->offsetY, z, 1.0f);

            _modelMatrices.push_back(modelMatrix);

            x += glyph->horiAdvance + kern.x;
            float shift = std::abs(x0 - static_cast<int>(x0));

            glyphInfo info;
            info.pos = glyph->texPos;
            info.size = glyph->texSize;
            info.shift = shift;

            info.page = glyph->texPage;

            _glyphInfos.push_back(info);

            previousGlyph = glyph;
        }
    }

    void textRenderPass::update(const vector<textRenderData>& texts)
    {
        _modelMatrices.clear();
        _glyphInfos.clear();

        for (auto& item : texts)
            addText(item.text, item.position, item.font);

        updateBuffers();
    }

    void textRenderPass::render() const
    {
        _glyphInfoBuffer->bindBufferBase(0);

        auto texelSize = 1.0f / (float)_fontManager->getGlyphAtlasSize();

        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendColor(1, 1, 1, 1);

        _shader->bind();
        _shader->setUniform(0, _viewMatrix);
        _shader->setUniform(1, _projectionMatrix);
        _shader->setUniform(2, _fontManager->getGlyphAtlasTextureAdress().unit);
        _shader->setUniform(3, glm::vec2(texelSize, texelSize));

        glBindVertexArray(_vao);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_glyphInfos.size()));
        glBindVertexArray(0);

        _shader->unbind();

        glBlendColor(0, 0, 0, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
}