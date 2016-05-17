#pragma once
#include <phi.h>
#include "uiApi.h"
#include <core\node.h>
#include <rendering\gl.h>
#include <rendering\camera.h>
#include <rendering\renderer.h>

#include "control.h"
#include "controlsRenderPass.h"
#include "glassyControlsRenderPass.h"
#include "textRenderPass.h"
#include "text.h"

namespace phi
{
    class uiRenderer
    {
    private:
        const gl* _gl;
        camera* _camera;
        vector<control*> _controls;
        vector<text*> _labels;
        controlsRenderPass* _controlsRenderPass;
        glassyControlsRenderPass* _glassyControlsRenderPass;
        textRenderPass* _textRenderPass;

    private:
        void addToLists(node* node);

    public:
        uiRenderer(renderer* renderer, gl* gl, camera* camera);
        ~uiRenderer();
        void add(node* node);
        void addControl(node* node, control* control);
        void addText(node* node, text* text);
        void update();
        void render() const;
    };
}