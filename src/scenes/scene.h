#pragma once

#include "scenes.h"

#include <core\globals.h>
#include <core\mathUtils.h>
#include <rendering\mesh.h>
#include <rendering\camera.h>
#include <rendering\pipeline.h>
#include <rendering\renderer.h>

#include <map>
#include <typeinfo>
#include <mutex>

namespace phi
{
    class scene
    {

    private:
        phi::gl* _gl;

        std::vector<object3D*> _objects;

        phi::renderer* _renderer;
        phi::pipeline* _pipeline;

    public:
        size_t w;
        size_t h;
        phi::camera* camera;

    public:
        SCENES_API scene(phi::gl* gl, size_t w, size_t h);
        SCENES_API ~scene();
        
        SCENES_API void update();
        SCENES_API void render();
        SCENES_API void resize(size_t w, size_t h);

        SCENES_API void add(object3D* object);
        SCENES_API void remove(object3D* object);
        SCENES_API float getZBufferValue(phi::ivec2 mousePos) 
        {
            auto pos = phi::ivec2(mousePos.x, h - mousePos.y);
            auto z = _renderer->defaultFramebuffer->getZBufferValue(pos);
            phi::debug("z buffer value: " + std::to_string(z));
            return z;
        }
    };
}