#include <precompiled.h>
#include "uiRenderer.h"

namespace phi
{
    uiRenderer::uiRenderer(renderer* renderer, gl* gl, camera* camera) :
        _gl(gl),
        _camera(camera),
        _controlsRenderPass(new controlsRenderPass(gl, _camera)),
        _glassyControlsRenderPass(new glassyControlsRenderPass(renderer->getFinalImageRT(), gl, _camera)),
        _textRenderPass(new textRenderPass(gl, _camera))
    {
    }

    uiRenderer::~uiRenderer()
    {
        safeDelete(_glassyControlsRenderPass);
        safeDelete(_controlsRenderPass);
        safeDelete(_textRenderPass);
    }

    void uiRenderer::add(node* node)
    {
        addToLists(node);
    }

    void uiRenderer::addToLists(node* node)
    {
        auto control = node->getComponent<phi::control>();

        if (control)
            addControl(node, control);

        auto text = node->getComponent<phi::text>();

        if (text)
            addText(node, text);

        auto children = node->getChildren();

        for (auto child : *children)
            addToLists(child);
    }

    void uiRenderer::addControl(node* node, control* controlRenderer)
    {
        _controlRenderers.push_back(controlRenderer);

        auto renderData = controlRenderData();
        renderData.backgroundColor = controlRenderer->getBackgroundColor();

        auto texture = controlRenderer->getBackgroundTexture();
        textureAddress address;

        auto texturesManager = _gl->texturesManager;

        if (texture == nullptr)
            texture = _gl->defaultAlbedoTexture;

        if (!texturesManager->contains(texture))
            address = texturesManager->add(texture);
        else
            address = texturesManager->get(texture);

        renderData.backgroundTextureUnit = address.unit;
        renderData.backgroundTexturePage = address.page;

        if (!controlRenderer->getIsGlassy())
            _controlsRenderPass->add(renderData, node->getTransform()->getModelMatrix());
        else
            _glassyControlsRenderPass->add(renderData, node->getTransform()->getModelMatrix());
    }

    void uiRenderer::addText(node* node, text* text)
    {
        _textRenderers.push_back(text);
        auto renderData = textRenderData();
        renderData.text = text->getText();
        renderData.position = node->getTransform()->getPosition();
        renderData.font = text->getFont();
        renderData.color = text->getColor();

        _textRenderPass->add(renderData);
    }

    void uiRenderer::update()
    {
        _controlsRenderPass->update();
    }

    void uiRenderer::render() const
    {
        _glassyControlsRenderPass->render();
        _controlsRenderPass->render();
        _textRenderPass->render();
    }
}