#pragma once
#include <phi.h>
#include "uiApi.h"
#include <rendering\gl.h>
#include "controlsRenderPass.h"
#include "textRenderPass.h"

namespace phi
{
    class uiRenderer
    {
    private:
        const gl* _gl;
        controlsRenderPass* _controlsRenderPass;
        textRenderPass* _textRenderPass;
        float _w;
        float _h;

    private:
        

    public:
        uiRenderer(const gl* const gl, float w, float h);
        ~uiRenderer();
        void update(const vector<control*>& controls);
        void update(const control* const control);
        void render() const;
    };
}