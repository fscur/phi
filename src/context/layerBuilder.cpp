#include <precompiled.h>
#include "layerBuilder.h"

#include <rendering\frameBuffer.h>
#include <rendering\texture.h>
#include <rendering\camera.h>
#include <rendering\glslCompiler.h>
#include <rendering\renderTargetsAddresses.h>
#include <rendering\vertexBuffer.h>

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
    map<string, shader*> layerBuilder::_shadersCache = map<string, shader*>();
    map<std::tuple<shader*, shader*>, program*> layerBuilder::_programsCache = map<std::tuple<shader*, shader*>, program*>();

    program* layerBuilder::buildProgram(
        const string& shadersPath,
        const string& programName,
        const string& vertexShaderName,
        const string& fragmentShaderName)
    {
        auto vertexShaderFileName = path::combine(shadersPath, vertexShaderName, shader::VERT_EXT);
        auto fragmentShaderFileName = path::combine(shadersPath, fragmentShaderName, shader::FRAG_EXT);

        auto newShader = [&](string fileName)
        {
            phi::shader* shader = nullptr;

            if (_shadersCache.find(fileName) != _shadersCache.end())
            {
                shader = _shadersCache[fileName];
            }
            else
            {
                shader = new phi::shader(fileName);
                _shadersCache[fileName] = shader;
#ifdef _DEBUG
                liveShaderReloader::shaders[fileName] = shader;
#endif
            }

            return shader;
        };

        auto vertexShader = newShader(vertexShaderFileName);
        auto fragmentShader = newShader(fragmentShaderFileName);

        phi::program* program = nullptr;
        auto key = std::tuple<shader*, shader*>(vertexShader, fragmentShader);

        if (_programsCache.find(key) != _programsCache.end())
            program = _programsCache[key];
        else
        {
            program = glslCompiler::compile({ vertexShader, fragmentShader });
            _programsCache[key] = program;
        }

        return program;
    }

    layer* layerBuilder::buildScene(const string& resourcesPath, gl* gl, float width, float height)
    {
        auto sceneCamera = new camera("sceneCamera", width, height, 0.1f, 10000.0f, PI_OVER_4);
        auto sceneLayer = new layer(sceneCamera);
        auto cameraPosition = vec3(0.0f, 0.0f, 10.0f);
        sceneCamera->getTransform()->setLocalPosition(cameraPosition);
        sceneCamera->getTransform()->setDirection(-cameraPosition);

        auto meshDescriptor = new meshRendererDescriptor(gl);
        auto meshRenderer = phi::meshRenderer::configure(gl, width, height, resourcesPath, meshDescriptor);

        sceneLayer->addOnNodeAdded([=](node* node)
        {
            auto mesh = node->getComponent<phi::mesh>();
            if (mesh)
            {
                sceneId::setNextId(mesh);
                meshDescriptor->add(mesh);
            }
        });

        sceneLayer->addOnUpdate([=] {});
        sceneLayer->addOnRender([=]
        {
            meshRenderer->render();
        });

        return sceneLayer;
    }

    layer* layerBuilder::buildUI(const string& resourcesPath, gl* gl, float width, float height)
    {
        auto shadersPath = path::combine(resourcesPath, "shaders");

        auto controlRenderer = new phi::controlRenderer(gl);
        auto textRenderer = new phi::textRenderer(gl);
        auto controlRenderPassProgram = buildProgram(shadersPath, "control", "control", "control");
        auto textRenderPassProgram = buildProgram(shadersPath, "text", "text", "text");

        auto uiCamera = new camera("uiCamera", width, height, 0.1f, 10000.0f, PI_OVER_4);
        auto uiLayer = new layer(uiCamera);
        uiCamera->getTransform()->setLocalPosition(vec3(0.0f, 0.0f, 400.0f));
        uiCamera->getTransform()->setDirection(vec3(0.0f, 0.0f, -1.0f));

        uiLayer->addOnNodeAdded([=](node* node)
        {
            auto control = node->getComponent<phi::control>();

            if (control)
                controlRenderer->add(control);
        });

        uiLayer->addOnNodeAdded([=](node* node)
        {
            auto text = node->getComponent<phi::text>();

            if (text)
                textRenderer->add(text);
        });

        uiLayer->addOnUpdate([=] {});
        uiLayer->addOnRender([=] { controlRenderer->render(controlRenderPassProgram); });
        uiLayer->addOnRender([=] { textRenderer->render(textRenderPassProgram); });

        return uiLayer;
    }
}