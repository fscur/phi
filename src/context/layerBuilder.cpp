#include <precompiled.h>
#include "layerBuilder.h"

#include <core\resolution.h>

#include <rendering\camera.h>

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
    layerBuilder::layerBuilder(layer* layer, gl* gl, resolution resolution, string resourcesPath) :
        _layer(layer),
        _gl(gl),
        _resolution(resolution),
        _resourcesPath(resourcesPath)
    {
    }

    layerBuilder layerBuilder::newLayer(camera* camera, gl* gl, resolution resolution, string resourcesPath)
    {
        return layerBuilder(new layer(camera), gl, resolution, resourcesPath);
    }

    layerBuilder layerBuilder::withMeshRenderer()
    {
        auto rendererDescriptor = new meshRendererDescriptor(_gl);
        _meshRenderPasses = phi::meshRenderer::configure(
            _gl,
            _resolution,
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

        for (auto& renderPass : _meshRenderPasses)
        {
            _layer->addRenderPass(renderPass);
        }

        return *this;
    }

    layerBuilder layerBuilder::withGlassyControlRenderer()
    {
        auto rendererDescriptor = new glassyControlRendererDescriptor(_gl, _resolution);
        _glassyControlRenderPasses = glassyControlRenderer::configure(
            _gl,
            _resourcesPath,
            rendererDescriptor);

        _layer->addOnNodeAdded([=](node* node)
        {
            auto control = node->getComponent<phi::control>();

            if (control)
                rendererDescriptor->add(control);
        });

        if (_meshRenderPasses.size() > 0) //TODO:Fix this gambi
        {
            auto rt = _meshRenderPasses.back()->getOuts()[0]->renderTarget;
            rendererDescriptor->updateGlassyUniformBlock(rt);
        }

        _layer->onInputChanged.assign([=] (layer* layer) //TODO:Fix this gambi
        {
            auto rt = layer->getOuts()[0]->renderTarget;
            rendererDescriptor->updateGlassyUniformBlock(rt);
        });

        for (auto& renderPass : _glassyControlRenderPasses)
        {
            _layer->addRenderPass(renderPass);
        }

        _layer->addOnUpdate([=] {});

        return *this;
    }

    layerBuilder layerBuilder::withControlRenderer()
    {
        auto rendererDescriptor = new controlRendererDescriptor(_gl);
        _controlRenderPasses = controlRenderer::configure(_gl, _resourcesPath, rendererDescriptor);

        _layer->addOnNodeAdded([=](node* node)
        {
            auto control = node->getComponent<phi::control>();

            if (control)
                rendererDescriptor->add(control);
        });

        for (auto& renderPass : _controlRenderPasses)
        {
            _layer->addRenderPass(renderPass);
        }

        _layer->addOnUpdate([=] {});

        return *this;
    }

    layerBuilder layerBuilder::withTextRenderer()
    {
        auto rendererDescriptor = new textRendererDescriptor(_gl);
        _textRenderPasses = phi::textRenderer::configure(_gl, _resourcesPath, rendererDescriptor);

        _layer->addOnNodeAdded([=](node* node)
        {
            auto text = node->getComponent<phi::text>();

            if (text)
                rendererDescriptor->add(text);
        });

        for (auto& renderPass : _textRenderPasses)
        {
            _layer->addRenderPass(renderPass);
        }

        _layer->addOnUpdate([=] {});

        return *this;
    }

    layer* layerBuilder::build()
    {
        return _layer;
    }
}