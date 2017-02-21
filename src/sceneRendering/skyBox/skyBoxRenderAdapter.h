#pragma once
#include <phi.h>
#include "../sceneRenderingApi.h"

#include <rendering\mappedVertexBuffer.h>
#include <rendering\mappedBuffer.h>
#include <rendering\shader.h>
#include <rendering\program.h>
#include <rendering\vertexArrayObject.h>

#include <core\skyBox.h>

#include "skyBoxRenderData.h"

namespace phi
{
    class skyBoxRenderAdapter
    {

    private:
        vertexArrayObject* _vao;
        mappedVertexBuffer<skyBox*, mat4>* _modelMatricesBuffer;
        mappedBuffer<skyBox*, skyBoxRenderData>* _skyBoxRenderDataBuffer;

    private:
        void createBuffers();

    public:
        SCENE_RENDERING_API skyBoxRenderAdapter();
        SCENE_RENDERING_API ~skyBoxRenderAdapter();

        SCENE_RENDERING_API void add(skyBox* skyBox);
        SCENE_RENDERING_API void remove(skyBox* skyBox);
        SCENE_RENDERING_API void update(skyBox* skyBox);

        vertexArrayObject* getVao() const { return _vao; };
        mappedBuffer<skyBox*, skyBoxRenderData>* getSkyBoxDataBuffer() const { return _skyBoxRenderDataBuffer; };
    };
}