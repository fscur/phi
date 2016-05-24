#include <precompiled.h>
#include "glassyControlsRenderPass.h"

namespace phi
{
    glassyControlsRenderPass::glassyControlsRenderPass(renderTarget* backgroundRenderTarget, gl* gl, camera* camera) :
        _gl(gl),
        _camera(camera),
        _texturesManager(gl->texturesManager),
        _shader(nullptr),
        _quad(nullptr),
        _vbo(nullptr),
        _modelMatricesBuffer(nullptr),
        _ebo(nullptr),
        _controlsRenderDataBuffer(nullptr),
        _backgroundRenderTarget(backgroundRenderTarget),
        _resolution(vec2(backgroundRenderTarget->texture->w, backgroundRenderTarget->texture->h)),
        _vao(0u),
        _modelMatrices(vector<mat4>()),
        _controlsRenderData(vector<controlRenderData>()),
        _instanceCount(0)
    {
        initShader();
        createQuad();
        createVao();
    }

    inline glassyControlsRenderPass::~glassyControlsRenderPass()
    {
        safeDelete(_quad);
        safeDelete(_vbo);
        safeDelete(_modelMatricesBuffer);
        safeDelete(_controlsRenderDataBuffer);
        safeDelete(_ebo);
    }

    void glassyControlsRenderPass::initShader()
    {
        vector<string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inModelMatrix");

        _shader = _gl->shadersManager->load("glassy", attribs);
        _shader->addUniform(0, "v");
        _shader->addUniform(1, "p");
        _shader->addUniform(2, "textureArrays");
        _shader->addUniform(3, "backgroundUnit");
        _shader->addUniform(4, "backgroundPage");
        _shader->addUniform(5, "resolution");
        _shader->addUniform(6, "level");
    }

    void glassyControlsRenderPass::createQuad()
    {
        auto vertices = vector<vertex>
        {
            vertex(vec3(0.0f, 0.0f, +0.0f), vec2(0.0f, 0.0f)),
            vertex(vec3(1.0f, 0.0f, +0.0f), vec2(1.0f, 0.0f)),
            vertex(vec3(1.0f, 1.0f, +0.0f), vec2(1.0f, 1.0f)),
            vertex(vec3(0.0f, 1.0f, +0.0f), vec2(0.0f, 1.0f))
        };

        auto indices = vector<uint>{ 0, 1, 2, 2, 3, 0 };

        _quad = geometry::create(vertices, indices);
    }

    void glassyControlsRenderPass::createVao()
    {
        glCreateVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        auto geometry = _quad;
        createVbo(geometry->vboData, geometry->vboSize);
        createEbo(geometry->eboData, geometry->eboSize);
        createModelMatricesBuffer();
        createControlsRenderDataBuffer();

        glBindVertexArray(0);
    }

    void glassyControlsRenderPass::createVbo(void* const data, GLsizeiptr size)
    {
        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        _vbo = new vertexBuffer(attribs);
        _vbo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void glassyControlsRenderPass::createModelMatricesBuffer()
    {
        vector<vertexAttrib> attribs;

        for (uint i = 0; i < 4; ++i)
            attribs.push_back(vertexAttrib(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new vertexBuffer(attribs);
        _modelMatricesBuffer->data(sizeof(mat4), nullptr, bufferDataUsage::dynamicDraw);
    }

    void glassyControlsRenderPass::createEbo(void* const data, GLsizeiptr size)
    {
        _ebo = new buffer(bufferTarget::element);
        _ebo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void glassyControlsRenderPass::createControlsRenderDataBuffer()
    {
        _controlsRenderDataBuffer = new buffer(bufferTarget::shader);
        _controlsRenderDataBuffer->data(sizeof(controlRenderData), nullptr, bufferDataUsage::dynamicDraw);
        _controlsRenderDataBuffer->bindBufferBase(0);
    }

    void glassyControlsRenderPass::updateBuffers()
    {
        _instanceCount = static_cast<GLsizei>(_modelMatrices.size());

        _modelMatricesBuffer->data(sizeof(mat4) * _instanceCount, &_modelMatrices[0], bufferDataUsage::dynamicDraw);
        _controlsRenderDataBuffer->data(sizeof(controlRenderData) * _instanceCount, &_controlsRenderData[0], bufferDataUsage::dynamicDraw);
    }

    void glassyControlsRenderPass::add(controlRenderData renderData, mat4 modelMatrix)
    {
        _controlsRenderData.push_back(renderData);
        _modelMatrices.push_back(modelMatrix);

        updateBuffers();
    }

    void glassyControlsRenderPass::update()
    {
        //updateBuffers();
    }

    void glassyControlsRenderPass::render() const
    {
        _controlsRenderDataBuffer->bindBufferBase(0);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendColor(1, 1, 1, 1);

        _shader->bind();
        _shader->setUniform(0, _camera->getViewMatrix());
        _shader->setUniform(1, _camera->getProjectionMatrix());
        _shader->setUniform(2, _texturesManager->units);
        _shader->setUniform(3, _backgroundRenderTarget->textureAddress.unit);
        _shader->setUniform(4, _backgroundRenderTarget->textureAddress.page);
        _shader->setUniform(5, _resolution);
        _shader->setUniform(6, 4);

        glBindVertexArray(_vao);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, _instanceCount);
        glBindVertexArray(0);

        _shader->unbind();
        glBlendColor(0, 0, 0, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
    }
}