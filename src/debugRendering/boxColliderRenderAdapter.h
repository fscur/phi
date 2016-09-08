#pragma once
#include <phi.h>
#include "debugRenderingApi.h"

#include <core\boxCollider.h>

#include <rendering\vertexBuffer.h>
#include <rendering\indexBuffer.h>
#include <rendering\vertexArrayObject.h>
#include <rendering\mappedVertexBuffer.h>

namespace phi
{
    class boxColliderRenderAdapter
    {
    private:
        vertexArrayObject* _vao;
        mappedVertexBuffer<boxCollider*, mat4>* _modelMatricesBuffer;

    private:
        void createBuffers();
        void createVao();
        void updateModelMatrix(boxCollider* node);

    public:
        DEBUG_RENDERING_API boxColliderRenderAdapter();
        DEBUG_RENDERING_API ~boxColliderRenderAdapter();
        
        DEBUG_RENDERING_API void add(boxCollider* node);
        DEBUG_RENDERING_API void remove(boxCollider* node);
        DEBUG_RENDERING_API void update(boxCollider* node);

        vertexArrayObject* getVao() const { return _vao; };

        mappedVertexBuffer<boxCollider*, mat4>* getModelMatricesBuffer() const { return _modelMatricesBuffer; }
    };
}