#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\mesh.h>
#include <core\resolution.h>

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

        vector<renderPassOut*> getOuts() const { return _renderPasses.back()->getOuts(); }
        vector<renderPass*> getRenderPasses() const { return _renderPasses; }

        static SCENE_RENDERING_API vector<renderPass*> configure(gl* gl, resolution resolution, const string& resourcesPath, meshRendererDescriptor* renderDescriptor);
    };
}