#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\mesh.h>

#include <rendering\gl.h>
#include <rendering\program.h>
#include <rendering\renderPass.h>
#include <rendering\frameBuffer.h>
#include <rendering\renderTarget.h>
#include <rendering\renderTargetsAddresses.h>

#include "meshRendererDescriptor.h"
#include "materialRenderData.h"
#include "batch.h"

namespace phi
{
    class meshRenderer
    {
    private:
        vector<renderPass*> _renderPasses;

    private:
        SCENE_RENDERING_API meshRenderer(vector<renderPass*>&& renderPasses);

    public:
        SCENE_RENDERING_API ~meshRenderer();
        SCENE_RENDERING_API void render();

        static SCENE_RENDERING_API meshRenderer* configure(gl* gl, float width, float height, const string& resourcesPath, meshRendererDescriptor* renderDescriptor);
    };
}