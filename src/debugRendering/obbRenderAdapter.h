#pragma once
#include <phi.h>
#include "debugRenderingApi.h"

#include <core\node.h>

#include <rendering\vertexBuffer.h>
#include <rendering\indexBuffer.h>
#include <rendering\vertexArrayObject.h>
#include <rendering\mappedVertexBuffer.h>
//#include "controlRenderData.h"

namespace phi
{
    class obbRenderAdapter
    {
    private:
        vertexArrayObject* _linesVao;
        vertexArrayObject* _boxVao;
        mappedVertexBuffer<node*, mat4>* _modelMatricesBuffer;

    private:
        void createBuffers();
        void createBoxVao();
        void createLinesVao();
        void updateModelMatrix(node* node);

    public:
        DEBUG_RENDERING_API obbRenderAdapter();
        DEBUG_RENDERING_API ~obbRenderAdapter();
        
        DEBUG_RENDERING_API void add(node* node);
        DEBUG_RENDERING_API void remove(node* node);
        DEBUG_RENDERING_API void update(node* node);

        vertexArrayObject* getLinesVao() const { return _linesVao; };
        vertexArrayObject* getBoxVao() const { return _boxVao; };

        mappedVertexBuffer<node*, mat4>* getModelMatricesBuffer() const { return _modelMatricesBuffer; }
    };
}