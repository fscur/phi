#pragma once
#include <phi.h>
#include <core\node.h>
#include <rendering\renderer.h>
#include <rendering\gl.h>
#include <rendering\camera.h>

#include "uiApi.h"
#include "uiRenderer.h"

namespace phi
{
    class ui
    {
    private:
        const gl* _gl;
        vector<node*> _controls;
        camera* _camera;
        uiRenderer* _renderer;
        float _w;
        float _h;

    public:
        UI_API node* newLabel(wstring text, vec3 position);

    public:
        UI_API ui(camera* camera, renderer* renderer, gl* gl, float w, float h);
        UI_API ~ui();
        
        UI_API void update();
        UI_API void render();

        UI_API void add(node* n);

    };
}