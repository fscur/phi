#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "framebuffer.h"
#include "program.h"
#include "vertexArrayObject.h"

namespace phi
{
    struct renderPassIn
    {
        renderTarget* renderTarget;
    };

    struct renderPassOut
    {
        renderPassOut(renderTarget* renderTarget) :
            renderTarget(renderTarget)
        {
        }

        renderTarget* renderTarget;
    };

    class renderPass
    {
    private:
        program* _program;
        vector<vertexArrayObject*> _vaos;
        //vector<buffer*> _buffers;
        framebuffer* _framebuffer;
        vector<renderPassIn*> _ins;
        vector<renderPassOut*> _outs;

        std::function<void(program*)> _onUpdate;
        std::function<void(program*, framebuffer*)> _onBeginRender;
        std::function<void(const vector<vertexArrayObject*>&)> _onRender;
        std::function<void(program*, framebuffer*)> _onEndRender;

    public:
        RENDERING_API renderPass(program* program, framebuffer* framebuffer);
        RENDERING_API ~renderPass();

        RENDERING_API void update();
        RENDERING_API void render();

        void setOnUpdate(std::function<void(program*)> onUpdate) { _onUpdate = onUpdate; }
        void setOnBeginRender(std::function<void(program*, framebuffer*)> onRender) { _onBeginRender = onRender; }
        void setOnRender(std::function<void(const vector<vertexArrayObject*>&)> onRender) { _onRender = onRender; }
        void setOnEndRender(std::function<void(program*, framebuffer*)> onRender) { _onEndRender = onRender; }

        void addVao(vertexArrayObject* vao) { _vaos.push_back(vao); }
        //void addBuffer(buffer* buffer) { _buffers.push_back(buffer); }
        void addIn(renderPassIn* in) { _ins.push_back(in); }
        void addOut(renderPassOut* out) { _outs.push_back(out); }

        vector<renderPassIn*> getIns() const { return _ins; }
        vector<renderPassOut*> getOuts() const { return _outs; }
    };
}