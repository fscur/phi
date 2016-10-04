#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "framebuffer.h"
#include "program.h"
#include "vertexArrayObject.h"

namespace phi
{
    class renderPass
    {
    private:
        program* _program;
        vector<vertexArrayObject*> _vaos;
        framebuffer* _framebuffer;
        resolution _resolution;

        std::function<void()> _onInitialize;
        std::function<void(program*)> _onUpdate;
        std::function<bool(void)> _onCanRender;
        std::function<void(program*, framebuffer*, const resolution&)> _onBeginRender;
        std::function<void(const vector<vertexArrayObject*>&)> _onRender;
        std::function<void(program*, framebuffer*, const resolution&)> _onEndRender;
        std::function<void(const resolution& resolution)> _onResize;
        std::function<void(void)> _onDelete;

    public:
        RENDERING_API renderPass(program* program, framebuffer* framebuffer, const resolution& resolution);
        RENDERING_API ~renderPass();

        RENDERING_API void initialize();
        RENDERING_API void update();
        RENDERING_API void render();

        void setOnInitialize(std::function<void()> onInitialize) { _onInitialize = onInitialize; }
        void setOnUpdate(std::function<void(program*)> onUpdate) { _onUpdate = onUpdate; }
        void setOnCanRender(std::function<bool(void)> onCanRender) { _onCanRender = onCanRender; }
        void setOnBeginRender(std::function<void(program*, framebuffer*, const resolution&)> onRender) { _onBeginRender = onRender; }
        void setOnRender(std::function<void(const vector<vertexArrayObject*>&)> onRender) { _onRender = onRender; }
        void setOnEndRender(std::function<void(program*, framebuffer*, const resolution&)> onRender) { _onEndRender = onRender; }
        void setOnResize(std::function<void(const resolution&)> onResize) { _onResize = onResize; }
        void setOnDelete(std::function<void(void)> onDelete) { _onDelete = onDelete; }

        void addVao(vertexArrayObject* vao) { _vaos.push_back(vao); }

        RENDERING_API void resize(const resolution& resolution);
    };
}