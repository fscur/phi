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
    protected:
        vector<vertexArrayObject*> _vaos;
        program* _program;
        framebuffer* _framebuffer;
        resolution _resolution;

    public:
        RENDERING_API renderPass(const resolution& resolution);
        RENDERING_API ~renderPass();

        RENDERING_API void initialize();
        RENDERING_API void update();
        RENDERING_API void render();

        RENDERING_API void resize(const resolution& resolution);

        void addVao(vertexArrayObject* vao) { _vaos.push_back(vao); }

        virtual void onInitialize() {}
        virtual void onUpdate() {}
        virtual void onBeginRender() {}
        RENDERING_API virtual void onRender();
        virtual void onEndRender() {}
        virtual void onResize(const resolution& resolution) { _unused(resolution); }
    };
}