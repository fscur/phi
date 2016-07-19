#include <precompiled.h>
#include "boxColliderRenderAdapter.h"

#include <core\notImplementedException.h>
#include <core\geometry.h>
#include <core\node.h>
#include <rendering\texturesManager.h>

namespace phi
{
    boxColliderRenderAdapter::boxColliderRenderAdapter()
    {
        createVao();
    }

    boxColliderRenderAdapter::~boxColliderRenderAdapter()
    {
        safeDelete(_vao);
    }

    void boxColliderRenderAdapter::createVao()
    {
        vec3 vertices[8] =
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

        vector<vertexBufferAttribute> verticesAttribs =
        {
            vertexBufferAttribute(0, 3, GL_FLOAT, sizeof(vec3), 0)
        };

        auto vbo = new vertexBuffer("vbo", verticesAttribs);
        vbo->storage(8 * sizeof(vec3), vertices, bufferStorageUsage::none);

        auto ebo = new indexBuffer("ebo");
        ebo->storage(24 * sizeof(uint32_t), indices, bufferStorageUsage::none);

        _modelMatricesBuffer = new mappedVertexBuffer<boxCollider*, mat4>("modelMatrices",
        {
            vertexBufferAttribute(1, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 0 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(2, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 1 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(3, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 2 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(4, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 3 * 4), 1)  // I'm not dumb (I think), I just trust the compiler
        });
        
        auto renderFunction = [&]
        {
            glDrawElementsInstanced(GL_LINES, 24, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_modelMatricesBuffer->getInstanceCount()));
        };

        _vao = new vertexArrayObject();
        _vao->addBuffer(vbo);
        _vao->addBuffer(ebo);
        _vao->addBuffer(_modelMatricesBuffer);
        _vao->setOnRender(renderFunction);
    }

    void boxColliderRenderAdapter::updateModelMatrix(boxCollider* boxCollider)
    {
        throw notImplementedException();
    }

    void boxColliderRenderAdapter::add(boxCollider* boxCollider)
    {
        auto nodeModelMatrix = boxCollider->getLocalModelMatrix();
        _modelMatricesBuffer->add(boxCollider, nodeModelMatrix);
    }

    void boxColliderRenderAdapter::remove(boxCollider* boxCollider)
    {
        _modelMatricesBuffer->remove(boxCollider);
    }

    void boxColliderRenderAdapter::update(boxCollider* boxCollider)
    {
        updateModelMatrix(boxCollider);
    }
}