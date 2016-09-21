#pragma once

#include <phi.h>

#include <rendering/camera.h>
#include <rendering/mappedVertexBuffer.h>
#include <rendering/mappedBuffer.h>
#include <rendering/program.h>
#include <rendering/shader.h>
#include <rendering/vertexArrayObject.h>

#include <ui/text.h>

#include "textInstance.h"
#include "uiRenderingApi.h"

namespace phi
{
    class textRenderAdapter
    {
    public:
        UI_RENDERING_API textRenderAdapter(camera* camera);
        UI_RENDERING_API ~textRenderAdapter();

        UI_RENDERING_API void add(text* text);
        UI_RENDERING_API void remove(text* text);
        UI_RENDERING_API void update(text* text);

        vertexArrayObject* getVao() const { return _vao; };
        mappedBuffer<text*, glyphRenderData>* getGlyphRenderDataBuffer() const { return _glyphRenderDataBuffer; };

    private:
        void createBuffers();
        textInstance createTextInstance(text* text);
        void onCameraChanged(transform* transform);

    private:
        vertexArrayObject* _vao;
        mappedVertexBuffer<text*, mat4>* _modelMatricesBuffer;
        mappedBuffer<text*, glyphRenderData>* _glyphRenderDataBuffer;
        vector<text*> _texts;
        camera* _camera;
        eventToken _cameraChangedEventToken;
    };
}