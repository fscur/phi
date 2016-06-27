#include <precompiled.h>

#include <core\node.h>

#include "boxColliderRenderPass.h"

namespace phi
{
    boxColliderRenderPass::boxColliderRenderPass(gl* gl) :
        _gl(gl),
        _collidersData(),
        _modelMatrices(nullptr),
        _vao(0u),
        _positionsBuffer(nullptr),
        _indicesBuffer(nullptr),
        _modelMatricesBuffer(nullptr)
    {
        createShader();
        createStorageData();
    }

    boxColliderRenderPass::~boxColliderRenderPass()
    {
        for (auto data : _collidersData)
            safeDelete(data);

        safeDelete(_positionsBuffer);
        safeDelete(_modelMatricesBuffer);
        safeDelete(_indicesBuffer);
        safeDelete(_modelMatrices);
    }

    void boxColliderRenderPass::addCollider(boxCollider* collider)
    {
        auto data = new boxColliderRenderData();
        data->collider = collider;
        data->transformChangedToken = collider->getNode()->getTransform()->getChangedEvent()->assign(
            [this, data](transform* sender)
        {
            updateModelMatrix(data);
            updateStorageData();
        });

        updateModelMatrix(data);
        _collidersData.push_back(data);
        updateStorageData();
    }

    void boxColliderRenderPass::removeCollider(boxCollider* collider)
    {
        _collidersData.erase(std::remove_if(_collidersData.begin(), _collidersData.end(),
            [&collider](const boxColliderRenderData* d)
        {
            return collider == d->collider;
        }), _collidersData.end());

        updateStorageData();
    }

    void boxColliderRenderPass::updateModelMatrix(boxColliderRenderData* data)
    {
        auto obb = data->collider->getObb();
        auto translation = glm::translate(obb.center);
        auto rotation = glm::mat4(
            vec4(obb.axes[0], 0.0f),
            vec4(obb.axes[1], 0.0f),
            vec4(obb.axes[2], 0.0f),
            vec4(0.0f, 0.0f, 0.0f, 1.0f));
        auto scale = glm::scale(obb.halfSizes * 2.0f);

        data->modelMatrix = translation * rotation * scale;
    }

    void boxColliderRenderPass::createShader()
    {
        vector<string> attribs =
        {
            "inPosition",
            "inModelMatrix"
        };

        _shader = _gl->shadersManager->load("boxColliderPass", attribs);
    }

    void boxColliderRenderPass::createStorageData()
    {
        vec3 points[8] =
        {
            vec3(-0.5f, -0.5f, -0.5f),
            vec3(-0.5f, -0.5f, 0.5f),
            vec3(-0.5f, 0.5f, 0.5f),
            vec3(-0.5f, 0.5f, -0.5f),
            vec3(0.5f, -0.5f, -0.5f),
            vec3(0.5f, -0.5f, 0.5f),
            vec3(0.5f, 0.5f, 0.5f),
            vec3(0.5f, 0.5f, -0.5f)
        };

        uint32_t indices[24] =
        {
            0, 1,
            1, 2,
            2, 3,
            3, 0,
            4, 5,
            5, 6,
            6, 7,
            7, 4,
            0, 4,
            1, 5,
            2, 6,
            3, 7
        };

        glCreateVertexArrays(1, &_vao);
        glError::check();

        glBindVertexArray(_vao);
        glError::check();

        _positionsBuffer = new vertexBuffer({ vertexAttrib(0, 3, GL_FLOAT, sizeof(vec3), 0) });
        _positionsBuffer->storage(8 * sizeof(vec3), points, bufferStorageUsage::none);

        _indicesBuffer = new buffer(bufferTarget::element);
        _indicesBuffer->storage(24 * sizeof(uint32_t), indices, bufferStorageUsage::none);

        _modelMatricesBuffer = new vertexBuffer(
            {
                vertexAttrib(1, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 0 * 4), 1), // I'm not dumb (I think), I just trust the compiler
                vertexAttrib(2, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 1 * 4), 1),
                vertexAttrib(3, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 2 * 4), 1),
                vertexAttrib(4, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 3 * 4), 1)
            });

        glBindVertexArray(0);
        glError::check();
    }

    void boxColliderRenderPass::updateStorageData()
    {
        if (_modelMatrices)
            safeDeleteArray(_modelMatrices);

        auto collidersCount = _collidersData.size();
        _modelMatrices = new mat4[collidersCount];
        for (size_t i = 0; i < collidersCount; i++)
            _modelMatrices[i] = _collidersData[i]->modelMatrix;

        _modelMatricesBuffer->data(sizeof(mat4) * collidersCount, _modelMatrices, phi::bufferDataUsage::staticDraw);
    }

    void boxColliderRenderPass::render()
    {
        glEnable(GL_DEPTH_TEST);
        glError::check();

        glBindVertexArray(_vao);
        glError::check();

        _shader->bind();

        glDrawElementsInstanced(GL_LINES, 24, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_collidersData.size()));
        glError::check();

        _shader->unbind();

        glBindVertexArray(0);
        glError::check();

        glDisable(GL_DEPTH_TEST);
        glError::check();
    }
}