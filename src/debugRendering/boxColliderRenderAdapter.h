#pragma once
#include <phi.h>
#include "debugRenderingApi.h"

#include <core\boxCollider.h>

#include <rendering\vertexBuffer.h>
#include <rendering\indexBuffer.h>
#include <rendering\vertexArrayObject.h>
#include <rendering\mappedVertexBuffer.h>
//#include "controlRenderData.h"

namespace phi
{
    class boxColliderRenderAdapter
    {
    private:
        vertexArrayObject* _linesVao;
        vertexArrayObject* _boxVao;
        mappedVertexBuffer<boxCollider*, mat4>* _modelMatricesBuffer;

    private:
        void createBuffers();
        void createBoxVao();
        void createLinesVao();
        void updateModelMatrix(boxCollider* boxCollider);

    public:
        DEBUG_RENDERING_API boxColliderRenderAdapter();
        DEBUG_RENDERING_API ~boxColliderRenderAdapter();
        
        DEBUG_RENDERING_API void add(boxCollider* boxCollider);
        DEBUG_RENDERING_API void remove(boxCollider* boxCollider);
        DEBUG_RENDERING_API void update(boxCollider* boxCollider);

        vertexArrayObject* getLinesVao() const { return _linesVao; };
        vertexArrayObject* getBoxVao() const { return _boxVao; };

        mappedVertexBuffer<boxCollider*, mat4>* getModelMatricesBuffer() const { return _modelMatricesBuffer; }
    };
}