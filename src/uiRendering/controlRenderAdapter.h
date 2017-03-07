#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <ui\control.h>

#include <rendering/buffer.h>
#include <rendering/camera.h>
#include <rendering/vertexBuffer.h>
#include <rendering/indexBuffer.h>
#include <rendering/mappedBuffer.h>
#include <rendering/mappedVertexBuffer.h>
#include <rendering/vertexArrayObject.h>

#include "controlRenderData.h"

namespace phi
{
    class controlRenderAdapter
    {
    public:
        UI_RENDERING_API controlRenderAdapter(camera* camera);
        UI_RENDERING_API ~controlRenderAdapter();

        UI_RENDERING_API void add(control* control);
        UI_RENDERING_API void remove(control* control);
        UI_RENDERING_API void update(control* control);
        UI_RENDERING_API void updateModelMatrix(control* control);

        vertexArrayObject* getVao() const { return _vao; };
        mappedBuffer<control*, controlRenderData>* getControlRenderDataBuffer() const { return _renderDataBuffer; };

    private:
        void createVao();
        geometry* createControlQuad();
        void createControlRenderDataBuffer();
        void updateControlRenderData(control* control);
        mat4 getModelMatrix(control* control);
        void onCameraChanged(transform* transform);

    private:
        const size_t MAX_CONTROL_INSTANCES = 1000;

        vertexArrayObject* _vao;
        mappedVertexBuffer<control*, mat4>* _modelMatricesBuffer;
        mappedVertexBuffer<control*, vec4>* _selectionColorBuffer;
        mappedBuffer<control*, controlRenderData>* _renderDataBuffer;
        vector<control*> _controls;
        camera* _camera;
        eventToken _cameraChangedEventToken;
    };
}