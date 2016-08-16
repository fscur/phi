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
        createBuffers();
    }

    boxColliderRenderAdapter::~boxColliderRenderAdapter()
    {
        safeDelete(_linesVao);
        safeDelete(_boxVao);
    }

    void boxColliderRenderAdapter::createBuffers()
    {
        _modelMatricesBuffer = new mappedVertexBuffer<boxCollider*, mat4>("modelMatrices",
        {
            vertexBufferAttribute(3, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 0 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(4, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 1 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(5, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 2 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(6, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 3 * 4), 1)  // I'm not dumb (I think), I just trust the compiler
        });

        createLinesVao();
        createBoxVao();
    }

    void boxColliderRenderAdapter::createLinesVao()
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

        auto renderFunction = [&]
        {
            glDrawElementsInstanced(GL_LINES, 24, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_modelMatricesBuffer->getInstanceCount()));
        };

        _linesVao = new vertexArrayObject();
        _linesVao->addBuffer(vbo);
        _linesVao->addBuffer(ebo);
        _linesVao->addBuffer(_modelMatricesBuffer);
        _linesVao->setOnRender(renderFunction);
    }

    void boxColliderRenderAdapter::createBoxVao()
    {
        auto box = geometry::createBox(1.0f);
        auto indicesCount = box->indicesCount;

        auto f0 = mat4(
            vec4(1.0, 0.0, 0.0, 0.0),
            vec4(0.0, 1.0, 0.0, 0.0),
            vec4(0.0, 0.0, 1.0, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0));

        /*auto f0 = mat4(
            vec4(2.0, 0.0, 0.0, 0.0),
            vec4(0.0, 2.0, 0.0, 0.0),
            vec4(0.0, 0.0, 2.0, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0));*/

        auto f1 = mat4(
            vec4(0.0, 0.0, 1.0, 0.0), 
            vec4(0.0, 1.0, 0.0, 0.0),
            vec4(-1.0, 0.0, 0.0, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0));

        auto f2 = mat4(
            vec4(-1.0, 0.0, 0.0, 0.0),
            vec4(0.0, 1.0, 0.0, 0.0),
            vec4(0.0, 0.0, -1.0, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0));

        auto f3 = mat4(
            vec4(0.0, 0.0, -1.0, 0.0),
            vec4(0.0, 1.0, 0.0, 0.0),
            vec4(1.0, 0.0, 0.0, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0));

        auto f4 = mat4(
            vec4(1.0, 0.0, 0.0, 0.0),
            vec4(0.0, 0.0, -1.0, 0.0),
            vec4(0.0, 1.0, 0.0, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0));

        auto f5 = mat4(
            vec4(1.0, 0.0, 0.0, 0.0),
            vec4(0.0, 0.0, 1.0, 0.0),
            vec4(0.0, -1.0, 0.0, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0));

        mat4 rotationMatrices[] = {
            f0, f0, f0, f0, 
            f1, f1, f1, f1,
            f2, f2, f2, f2,
            f3, f3, f3, f3,
            f4, f4, f4, f4,
            f5, f5, f5, f5
        };

        /*mat4 rotationMatrices[] = {
            f0, f0, f0, f0,
            f0, f0, f0, f0,
            f0, f0, f0, f0,
            f0, f0, f0, f0,
            f0, f0, f0, f0,
            f0, f0, f0, f0
        };*/

        auto rotationMatricesBuffer = new vertexBuffer("rotationMatricesBuffer",
        {
            vertexBufferAttribute(7, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 0 * 4), 0), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(8, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 1 * 4), 0), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(9, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 2 * 4), 0), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(10, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 3 * 4), 0)  // I'm not dumb (I think), I just trust the compiler

        });

        rotationMatricesBuffer->data(sizeof(mat4) * 24, rotationMatrices, bufferDataUsage::staticDraw);

        //auto rotationMatricesBuffer = new mappedVertexBuffer<boxCollider*, mat4>("modelMatrices",
        //{
        //    vertexBufferAttribute(3, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 0 * 4), 1), // I'm not dumb (I think), I just trust the compiler
        //    vertexBufferAttribute(4, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 1 * 4), 1), // I'm not dumb (I think), I just trust the compiler
        //    vertexBufferAttribute(5, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 2 * 4), 1), // I'm not dumb (I think), I just trust the compiler
        //    vertexBufferAttribute(6, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 3 * 4), 1)  // I'm not dumb (I think), I just trust the compiler
        //});

        auto renderFunction = [=]
        {
            float A = 0.0;
            float B = PI * 0.5;
            float C = 0.0;
            float cA = cos(A);
            float sA = sin(A);
            float cB = cos(B);
            float sB = sin(B);
            float cC = cos(C);
            float sC = sin(C);

            mat3 rx = mat3(
                vec3(1.0, 0.0, 0.0),
                vec3(0.0, cA, -sA),
                vec3(0.0, sA, cA));

            mat3 ry = mat3(
                vec3(cB, 0.0, sB),
                vec3(0.0, 1.0, 0.0),
                vec3(-sB, 0.0, cB));

            mat3 rz = mat3(
                vec3(cC, -sC, 0.0),
                vec3(sC, cC, 0.0),
                vec3(0.0, 0.0, 1.0));

            mat3 r = rx * ry * rz;

            glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_modelMatricesBuffer->getInstanceCount()));
        };

        _boxVao = vertexArrayObject::createBoxVao(box, renderFunction);
        _boxVao->addBuffer(_modelMatricesBuffer);
        _boxVao->addBuffer(rotationMatricesBuffer);
        safeDelete(box);
    }

    void boxColliderRenderAdapter::updateModelMatrix(boxCollider* boxCollider)
    {
        auto modelMatrix = boxCollider->getModelMatrix();
        _modelMatricesBuffer->update(boxCollider, modelMatrix);
    }

    void boxColliderRenderAdapter::add(boxCollider* boxCollider)
    {
        auto modelMatrix = boxCollider->getModelMatrix();
        _modelMatricesBuffer->add(boxCollider, modelMatrix);
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