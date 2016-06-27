#pragma once

#include <phi.h>

#include <core\boxCollider.h>
#include <rendering\vertexBuffer.h>

namespace phi
{
    class boxColliderRenderPass
    {
    private:
        struct boxColliderRenderData
        {
            boxCollider* collider;
            mat4 modelMatrix;
            eventToken transformChangedToken;
        };

    private:
        gl* _gl;
        vector<boxColliderRenderData*> _collidersData;
        mat4* _modelMatrices;
        uint _vao;
        vertexBuffer* _positionsBuffer;
        vertexBuffer* _modelMatricesBuffer;
        buffer* _indicesBuffer;
        shader* _shader;

    public:
        boxColliderRenderPass(gl* gl);
        ~boxColliderRenderPass();

    private:
        void createShader();
        void createStorageData();
        void updateStorageData();
        void updateModelMatrix(boxColliderRenderData* data);

    public:
        RENDERING_API void addCollider(boxCollider* collider);
        RENDERING_API void removeCollider(boxCollider* collider);

        RENDERING_API void render();
    };
}