#include <precompiled.h>
#include "obbRenderAdapter.h"

#include <core\notImplementedException.h>
#include <core\geometry.h>
#include <core\node.h>
#include <rendering\texturesManager.h>

namespace phi
{
    obbRenderAdapter::obbRenderAdapter()
    {
        createBuffers();
    }

    obbRenderAdapter::~obbRenderAdapter()
    {
        safeDelete(_linesVao);
        safeDelete(_boxVao);
    }

    void obbRenderAdapter::createBuffers()
    {
        _modelMatricesBuffer = new mappedVertexBuffer<node*, mat4>("modelMatrices",
        {
            vertexBufferAttribute(3, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 0 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(4, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 1 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(5, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 2 * 4), 1), // I'm not dumb (I think), I just trust the compiler
            vertexBufferAttribute(6, 4, GL_FLOAT, sizeof(mat4), (const void *)(sizeof(GLfloat) * 3 * 4), 1)  // I'm not dumb (I think), I just trust the compiler
        });

        createLinesVao();
        createBoxVao();
    }

    void obbRenderAdapter::createLinesVao()
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

    void obbRenderAdapter::createBoxVao()
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
            glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(_modelMatricesBuffer->getInstanceCount()));
        };

        _boxVao = vertexArrayObject::createBoxVao(box, renderFunction);
        _boxVao->addBuffer(_modelMatricesBuffer);
        _boxVao->addBuffer(rotationMatricesBuffer);
        safeDelete(box);
    }

    void obbRenderAdapter::updateModelMatrix(node* node)
    {
        auto modelMatrix = node->getObb()->toModelMatrix();
        _modelMatricesBuffer->update(node, modelMatrix);
    }

    void obbRenderAdapter::add(node* node)
    {
        auto modelMatrix = node->getObb()->toModelMatrix();
        _modelMatricesBuffer->add(node, modelMatrix);
    }

    void obbRenderAdapter::remove(node* node)
    {
        _modelMatricesBuffer->remove(node);
    }

    void obbRenderAdapter::update(node* node)
    {
        updateModelMatrix(node);
    }
}