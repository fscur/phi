#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <core\resolution.h>

#include <ui\control.h>

#include <rendering\buffer.h>
#include <rendering\vertexBuffer.h>
#include <rendering\mappedBuffer.h>
#include <rendering\mappedVertexBuffer.h>
#include <rendering\vertexArrayObject.h>
#include <rendering\renderTarget.h>

#include "controlRenderData.h"

namespace phi
{
    class glassyControlRenderAdapter
    {
    private:
        

    private:
        resolution _resolution;
        vertexBuffer* _modelMatricesBuffer;

    public:
        vertexArrayObject* _vao;
        buffer* _controlsRenderDataBuffer;

    private:
        void createBuffers();
        void updateModelMatrix(control* control);
        void updateControlRenderData(control* control);

    public:
        UI_RENDERING_API glassyControlRenderAdapter(resolution resolution);
        UI_RENDERING_API ~glassyControlRenderAdapter();

        UI_RENDERING_API void add(control* control);
        UI_RENDERING_API void remove(control* control);
        UI_RENDERING_API void update(control* control);
    };
}