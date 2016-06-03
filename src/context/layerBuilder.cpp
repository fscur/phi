#include <precompiled.h>
#include "layerBuilder.h"

#include <rendering\texture.h>
#include <rendering\camera.h>
#include <rendering\programBuilder.h>

#ifdef _DEBUG
#include <rendering\liveShaderReloader.h>
#endif

#include <sceneRendering\pipeline.h>

#include <sceneRendering\meshRenderer.h>
#include <uiRendering\controlRenderer.h>
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
        auto meshDescriptor = new meshRendererDescriptor(_gl);
        auto meshRenderer = phi::meshRenderer::configure(_gl, _width, _height, _resourcesPath, meshDescriptor);

        _layer->addOnNodeAdded([=](node* node)
        {
            auto mesh = node->getComponent<phi::mesh>();
            if (mesh)
            {
                sceneId::setNextId(mesh);
                meshDescriptor->add(mesh);
            }
        });

        _layer->addOnUpdate([=] {});
        _layer->addOnRender([=]
        {
            meshRenderer->render();
        });

        return *this;
    }

    layerBuilder layerBuilder::withControlRenderer()
    {
        auto shadersPath = path::combine(_resourcesPath, "shaders");

        auto controlRenderer = new phi::controlRenderer(_gl);
        auto controlRenderPassProgram = programBuilder::buildProgram(shadersPath, "control", "control");

        _layer->addOnNodeAdded([=](node* node)
        {
            auto control = node->getComponent<phi::control>();

            if (control)
                controlRenderer->add(control);
        });

        _layer->addOnUpdate([=] {});
        _layer->addOnRender([=] { controlRenderer->render(controlRenderPassProgram); });

        return *this;
    }

    layerBuilder layerBuilder::withTextRenderer()
    {
        auto shadersPath = path::combine(_resourcesPath, "shaders");
        auto textRenderer = new phi::textRenderer(_gl);
        auto textRenderPassProgram = programBuilder::buildProgram(shadersPath, "text", "text");

        _layer->addOnNodeAdded([=](node* node)
        {
            auto text = node->getComponent<phi::text>();

            if (text)
                textRenderer->add(text);
        });

        _layer->addOnUpdate([=] {});
        _layer->addOnRender([=] { textRenderer->render(textRenderPassProgram); });

        return *this;
    }

    layer* layerBuilder::build()
    {
        return _layer;
    }
}