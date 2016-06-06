#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "vao.h"

namespace phi
{
    class instancedVao :
        public vao
    {
    private:
        GLuint _id;
        GLsizei _instances;
        vertexBuffer* _modelMatricesBuffer;

    private:
        void create();

    public:
        RENDERING_API instancedVao();
        RENDERING_API ~instancedVao();

        RENDERING_API void update(const vector<mat4>& modelMatrices);

        RENDERING_API void render() override;
    };
}