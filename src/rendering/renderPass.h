#pragma once
#include <phi.h>
#include "renderingApi.h"

#include "shader.h"

namespace phi
{
    class renderPass
    {
    private:
        shader* _shader;
        std::function<void(shader*)> _onUpdate;
        std::function<void(shader*)> _onRender;
    public:
    public:
        RENDERING_API renderPass(shader* shader);
        RENDERING_API ~renderPass();

        RENDERING_API void update();
        RENDERING_API void render();

        void setOnUpdate(std::function<void(shader*)> onUpdate) { _onUpdate = onUpdate; }
        void setOnRender(std::function<void(shader*)> onRender) { _onRender = onRender; }
    };
}