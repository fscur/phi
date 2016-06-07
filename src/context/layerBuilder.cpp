#include <precompiled.h>
#include "layerBuilder.h"

#include <rendering\camera.h>

#ifdef _DEBUG
#include <rendering\liveShaderReloader.h>
#endif

#include <sceneRendering\meshRenderer.h>
#include <uiRendering\controlRenderer.h>
#include <uiRendering\glassyControlRenderer.h>
#include <uiRendering\textRenderer.h>

#include <context\sceneId.h>

#include <ui\control.h>
#include <ui\text.h>

#include <io\path.h>

namespace phi
{
    layerBuilder::layerBuilder(layer* layer, gl* gl, float width, float height, string resourcesPath) :
        _layer(layer),
        _gl(gl),
        _width(width),
        _height(height),
        _resourcesPath(resourcesPath)
    {
    }

    layerBuilder layerBuilder::newLayer(camera* camera, gl* gl, float width, float height, string resourcesPath)
    {
        return layerBuilder(new layer(camera), gl, width, height, resourcesPath);
    }

    layerBuilder layerBuilder::withMeshRenderer()
    {
        auto rendererDescriptor = new meshRendererDescriptor(_gl);
        auto renderer = phi::meshRenderer::configure(
            _gl, 
            _width, 
            _height, 
            _resourcesPath, 
            rendererDescriptor);

        _layer->addOnNodeAdded([=](node* node)
        {
            auto mesh = node->getComponent<phi::mesh>();
            if (mesh)
            {
                sceneId::setNextId(mesh);
                rendererDescriptor->add(mesh);
            }
        });

        _layer->addOnUpdate([=] {});
        _layer->addOnRender([=]
        {
            renderer->render();
        });

        return *this;
    }

    layerBuilder layerBuilder::withGlassyControlRenderer(renderPass* lastRenderPass)
    {
        auto rendererDescriptor = new glassyControlRendererDescriptor(_gl);
        auto renderer = glassyControlRenderer::configure(
            _gl, 
            _width, 
            _height, 
            _resourcesPath, 
            rendererDescriptor,
            lastRenderPass);

        _layer->addOnNodeAdded([=](node* node)
        {
            auto control = node->getComponent<phi::control>();

            if (control)
                rendererDescriptor->add(control);
        });

        _layer->addOnUpdate([=] {});
        _layer->addOnRender([=] { renderer->render(); });

        return *this;
    }

    layerBuilder layerBuilder::withControlRenderer()
    {
        auto rendererDescriptor = new controlRendererDescriptor(_gl);
        auto renderer = controlRenderer::configure(_gl, _width, _height, _resourcesPath, rendererDescriptor);

        _layer->addOnNodeAdded([=](node* node)
        {
            auto control = node->getComponent<phi::control>();

            if (control)
                rendererDescriptor->add(control);
        });

        _layer->addOnUpdate([=] {});
        _layer->addOnRender([=] { renderer->render(); });

        return *this;
    }

    layerBuilder layerBuilder::withTextRenderer()
    {
        auto rendererDescriptor = new textRendererDescriptor(_gl);
        auto renderer = phi::textRenderer::configure(_gl, _width, _height, _resourcesPath, rendererDescriptor);

        _layer->addOnNodeAdded([=](node* node)
        {
            auto text = node->getComponent<phi::text>();

            if (text)
                rendererDescriptor->add(text);
        });

        _layer->addOnUpdate([=] {});
        _layer->addOnRender([=] { renderer->render(); });

        return *this;
    }

    layer* layerBuilder::build()
    {
        return _layer;
    }
}