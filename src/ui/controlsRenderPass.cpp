#include <precompiled.h>
#include "controlsRenderPass.h"

namespace phi 
{ 
    controlsRenderPass::controlsRenderPass(const gl* const gl, float w, float h) :
        _gl(gl),
        _w(w),
        _h(h),
        _instanceCount(0)
    {
        initShader();
        initCamera();
        createQuad();
        createVao();
    }

    inline controlsRenderPass::~controlsRenderPass()
    {
        safeDelete(_quad);
        safeDelete(_shader);
        safeDelete(_vbo);
        safeDelete(_modelMatricesBuffer);
        safeDelete(_ebo);
    }

    void controlsRenderPass::initShader()
    {
        vector<string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inModelMatrix");

        _shader = _gl->shadersManager->load("control", attribs);
        _shader->addUniform(0, "v");
        _shader->addUniform(1, "p");
        _shader->addUniform(2, "textureArrays");
    }

    void controlsRenderPass::initCamera()
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

    void controlsRenderPass::createQuad()
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

    void controlsRenderPass::createVao()
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

    void controlsRenderPass::createVbo(void* const data, GLsizeiptr size)
    {
        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        _vbo = new vertexBuffer(attribs);
        _vbo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void controlsRenderPass::createModelMatricesBuffer()
    {
        vector<vertexAttrib> attribs;

        for (uint i = 0; i < 4; ++i)
            attribs.push_back(vertexAttrib(2 + i, 4, GL_FLOAT, sizeof(mat4), (const void*)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new vertexBuffer(attribs);
        _modelMatricesBuffer->data(sizeof(mat4), nullptr, bufferDataUsage::dynamicDraw);
    }

    void controlsRenderPass::createEbo(void* const data, GLsizeiptr size)
    {
        _ebo = new buffer(bufferTarget::element);
        _ebo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void controlsRenderPass::createControlsRenderDataBuffer()
    {
        _controlsRenderDataBuffer = new buffer(bufferTarget::shader);
        _controlsRenderDataBuffer->data(sizeof(controlRenderData), nullptr, bufferDataUsage::dynamicDraw);
        _controlsRenderDataBuffer->bindBufferBase(0);
    }

    void controlsRenderPass::update(const vector<control*>& controls)
    {
        _instanceCount = static_cast<GLsizei>(controls.size());
        _controlsRenderData.clear();
        _modelMatrices.clear();

        for (auto control : controls)
        {
            vec3 pos = control->getPosition();
            sizef size = control->getSize();

            auto modelMatrix = mat4(
                size.w, 0.0f, 0.0f, 0.0f,
                0.0f, size.h, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                pos.x, -pos.y, pos.z, 1.0f
            );

            _modelMatrices.push_back(modelMatrix);

            auto renderData = controlRenderData();
            renderData.backgroundColor = control->getBackgroundColor();

            auto texture = control->getBackgroundTexture();
            textureAddress address;

            auto texturesManager = _gl->texturesManager;

            if (texture == nullptr)
                texture = _gl->defaultAlbedoTexture;

            if (!texturesManager->contains(texture))
                address = texturesManager->add(texture);
            else
                address = texturesManager->get(texture);

            renderData.backgroundTextureUnit = address.unit;
            renderData.backgroundTexturePage = address.page;

            _controlsRenderData.push_back(renderData);
        }

        _modelMatricesBuffer->data(sizeof(mat4) * _instanceCount, &_modelMatrices[0], bufferDataUsage::dynamicDraw);
        _controlsRenderDataBuffer->data(sizeof(controlRenderData) * _instanceCount, &_controlsRenderData[0], bufferDataUsage::dynamicDraw);
    }

    void controlsRenderPass::render() const
    {
        _controlsRenderDataBuffer->bindBufferBase(0);

        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendColor(1, 1, 1, 1);

        _shader->bind();
        _shader->setUniform(0, _viewMatrix);
        _shader->setUniform(1, _projectionMatrix);
        _shader->setUniform(2, _gl->texturesManager->units);

        glBindVertexArray(_vao);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, _instanceCount);
        glBindVertexArray(0);

        _shader->unbind();
        glBlendColor(0, 0, 0, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
}