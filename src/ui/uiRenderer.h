#pragma once
#include <phi.h>
#include <core\node.h>
#include <rendering\gl.h>
#include <rendering\camera.h>
#include <rendering\renderer.h>

#include "uiApi.h"
#include "controlRenderer.h"
#include "controlsRenderPass.h"
#include "glassyControlsRenderPass.h"
#include "textRenderPass.h"
#include "textRenderer.h"

namespace phi
{
    class uiRenderer
    {
    private:
        const gl* _gl;
        camera* _camera;
		renderer* _renderer;
        vector<controlRenderer*> _controlRenderers;
        vector<textRenderer*> _textRenderers;
        controlsRenderPass* _controlsRenderPass;
		glassyControlsRenderPass* _glassyControlsRenderPass;
        textRenderPass* _textRenderPass;
		map<image*, texture*> _imageTextures;

    private:
        void addToLists(node* node);

    public:
        uiRenderer(renderer* renderer, gl* gl, camera* camera);
        ~uiRenderer();
        void add(node* node);
        void addControlRenderer(node* node, controlRenderer* controlRenderer);
        void addTextRenderer(node* node, textRenderer* textRenderer);
        void update();
        void render() const;
    };
}