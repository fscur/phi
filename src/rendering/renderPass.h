#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "program.h"

namespace phi
{
    class renderPass
    {
    private:
        program* _program;
        std::function<void(program*)> _onUpdate;
        std::function<void(program*)> _onRender;

    public:
        RENDERING_API renderPass(program* program);
        RENDERING_API ~renderPass();

        RENDERING_API void update();
        RENDERING_API void render();

        void setOnUpdate(std::function<void(program*)> onUpdate) { _onUpdate = onUpdate; }
        void setOnRender(std::function<void(program*)> onRender) { _onRender = onRender; }
    };
}