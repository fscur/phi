#include <precompiled.h>
#include "textRenderPass.h"

namespace phi
{
    inline textRenderPass::textRenderPass(gl* gl, camera* camera) :
        _gl(gl),
        _camera(camera),
        _fontsManager(gl->fontsManager),
        _texturesManager(gl->texturesManager),
        _shader(nullptr),
        _quad(nullptr),
        _vbo(nullptr),
        _modelMatricesBuffer(nullptr),
        _glyphIdsBuffer(nullptr),
        _ebo(nullptr),
        _glyphInfoBuffer(nullptr),
        _vao(0u),
        _modelMatrices(vector<mat4>()),
        _glyphInfos(vector<glyphInfo>())
    {
        initShader();
        createQuad();
        createVao();
    }

    inline textRenderPass::~textRenderPass()
    {
        safeDelete(_quad);
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
        _shader->addUniform(2, "textureArrays");
        _shader->addUniform(3, "texelSize");
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

    void textRenderPass::addText(const textRenderData& renderData)
    {
        auto font = renderData.font;

        if (font == nullptr)
            font = _gl->defaultFont;

        float baseLine = font->getBaseLine();
        float spacing = font->getSpacing();
        float x = renderData.position.x + spacing;
        float y = renderData.position.y - baseLine + spacing;
        float z = renderData.position.z;

        glyph* previousGlyph = nullptr;
        size_t textLength = renderData.text.length();

        auto glyph = _fontsManager->getGlyph(font, (ulong)renderData.text[0]);
        x -= glyph->offsetX;

        for (size_t i = 0; i < textLength; i++)
        {
            glyph = _fontsManager->getGlyph(font, (ulong)renderData.text[i]);
            auto kern = font->getKerning(previousGlyph, glyph);
            auto w = glyph->width;
            auto h = glyph->height;
            auto x0 = x + glyph->offsetX;
            auto y0 = y - h + glyph->offsetY;

            auto modelMatrix = mat4(
                w , 0.0f, 0.0f, 0.0f,
                0.0f, h, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                x0, y0, z, 1.0f);

            _modelMatrices.push_back(modelMatrix);

            x += glyph->horiAdvance + kern.x;

            float shift = std::abs(x0 - static_cast<int>(x0));

            glyphInfo info;
            info.pos = glyph->texPos;
            info.size = glyph->texSize;
            info.shift = shift;
            info.unit = glyph->texUnit;
            info.page = glyph->texPage;
            info.color = renderData.color;

            _glyphInfos.push_back(info);

            previousGlyph = glyph;
        }
    }

    void textRenderPass::add(const textRenderData& renderData)
    {
        addText(renderData);
        updateBuffers();
    }

    void textRenderPass::update(const vector<textRenderData>& texts)
    {
        _modelMatrices.clear();
        _glyphInfos.clear();

        for (auto& item : texts)
            addText(item);

        updateBuffers();
    }

    void textRenderPass::render() const
    {
        _glyphInfoBuffer->bindBufferBase(0);

        auto texelSize = 1.0f / (float)_fontsManager->getGlyphAtlasSize();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendColor(1, 1, 1, 1);

        _shader->bind();
        _shader->setUniform(0, _camera->getViewMatrix());
        _shader->setUniform(1, _camera->getProjectionMatrix());
        _shader->setUniform(2, _texturesManager->units);
        _shader->setUniform(3, glm::vec2(texelSize, texelSize));

        glBindVertexArray(_vao);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_glyphInfos.size()));
        glBindVertexArray(0);

        _shader->unbind();

        glBlendColor(0, 0, 0, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_CULL_FACE);
    }
}