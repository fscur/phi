#include <precompiled.h>
#include "uiRenderer.h"
//#include "label.h"

namespace phi
{
    uiRenderer::uiRenderer(gl* gl, camera* camera) :
        _gl(gl),
        _camera(camera), 
        _controlsRenderPass(new controlsRenderPass(gl, _camera)),
        _textRenderPass(new textRenderPass(gl, _camera))
    {   
    }

    uiRenderer::~uiRenderer()
    {
        safeDelete(_controlsRenderPass);
        safeDelete(_textRenderPass);
    }

    /*void uiRenderer::update(const vector<control*>& controls)
    {
        _controlsRenderPass->update(controls);
        
        vector<textRenderData> texts;

        for (auto control : controls)
        {
            switch (control->type)
            {
                case controlType::label:
                {
                    textRenderData data;
                    auto label = static_cast<phi::label*>(control);
                    data.font = label->getFont();
                    data.position = label->getPosition();
                    data.text = label->getText();
                    texts.push_back(data);
                }
                break;
            default:
                break;
            }
        }

        _textRenderPass->update(texts);
    }
    
    void uiRenderer::update(const control* const control)
    {
    }
*/

    void uiRenderer::add(node* node)
    {
        addToLists(node);
    }

    void uiRenderer::addToLists(node* node)
    {
        auto controlRenderer = node->getComponent<phi::controlRenderer>();

        if (controlRenderer)
            addControlRenderer(node, controlRenderer);

        auto textRenderer = node->getComponent<phi::textRenderer>();

        if (textRenderer)
            addTextRenderer(node, textRenderer);

        auto children = node->getChildren();

        for (auto child : children)
            addToLists(child);
    }

    void uiRenderer::addControlRenderer(node* node, controlRenderer* controlRenderer)
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

        _controlsRenderPass->add(renderData, node->getTransform()->getModelMatrix());
    }

    void uiRenderer::addTextRenderer(node* node, textRenderer* textRenderer)
    {
        _textRenderers.push_back(textRenderer);
        auto renderData = textRenderData();
        renderData.text = textRenderer->getText();
        renderData.position = node->getTransform()->getPosition();
        renderData.font = textRenderer->getFont();

        _textRenderPass->add(renderData);
    }

    void uiRenderer::update()
    {
        _controlsRenderPass->update();
    }

    void uiRenderer::render() const
    {   
        _controlsRenderPass->render();
        _textRenderPass->render();
    }
}