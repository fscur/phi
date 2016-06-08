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
        _meshRenderPasses = phi::meshRenderer::configure(
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

        for (auto& renderPass : _meshRenderPasses)
        {
            _layer->addRenderPass(renderPass);
        }

        return *this;
    }

    layerBuilder layerBuilder::withGlassyControlRenderer()
    {
        auto rendererDescriptor = new glassyControlRendererDescriptor(_gl, _width, _height);
        _glassyControlRenderPasses = glassyControlRenderer::configure(
            _gl, 
            _width, 
            _height, 
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
        _controlRenderPasses = controlRenderer::configure(_gl, _width, _height, _resourcesPath, rendererDescriptor);

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
        _textRenderPasses = phi::textRenderer::configure(_gl, _width, _height, _resourcesPath, rendererDescriptor);

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