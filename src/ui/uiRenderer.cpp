#include <precompiled.h>
#include "uiRenderer.h"
//#include "label.h"

namespace phi
{
    uiRenderer::uiRenderer(renderer* renderer, gl* gl, camera* camera) :
        _gl(gl),
        _camera(camera), 
		_renderer(renderer),
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

		for (auto pair : _imageTextures)
			safeDelete(pair.second);
    }

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

        for (auto child : *children)
            addToLists(child);
    }

    void uiRenderer::addControlRenderer(node* node, controlRenderer* controlRenderer)
    {
        _controlRenderers.push_back(controlRenderer);

        auto renderData = controlRenderData();
        renderData.backgroundColor = controlRenderer->getBackgroundColor();

        auto image = controlRenderer->getBackgroundImage();
		if (image == nullptr)
			image = _gl->defaultAlbedoImage;

		phi::texture* texture = nullptr;

		if (_imageTextures.find(image) != _imageTextures.end())
			texture = _imageTextures[image];
		else
		{
			texture = new phi::texture(
				image,
				GL_TEXTURE_2D,
				GL_RGBA8,
				GL_REPEAT,
				GL_LINEAR_MIPMAP_LINEAR,
				GL_LINEAR,
				true);

			_imageTextures[image] = texture;
		}

        textureAddress address;

        auto texturesManager = _gl->texturesManager;

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

    void uiRenderer::addTextRenderer(node* node, textRenderer* textRenderer)
    {
        _textRenderers.push_back(textRenderer);
        auto renderData = textRenderData();
        renderData.text = textRenderer->getText();
        renderData.position = node->getTransform()->getPosition();
        renderData.font = textRenderer->getFont();
        renderData.color = textRenderer->getColor();

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