#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\mesh.h>
#include <core\resolution.h>

#include <rendering\program.h>
#include <rendering\renderPass.h>
#include <rendering\frameBuffer.h>
#include <rendering\renderTarget.h>
#include <rendering\renderTargetsAddresses.h>
#include <rendering\framebufferAllocator.h>

#include "meshRendererDescriptor.h"
#include "materialRenderData.h"
#include "batch.h"

namespace phi
{
    class meshRenderer
    {
    public:
        meshRenderer() = delete;
        ~meshRenderer() = delete;

        static SCENE_RENDERING_API vector<renderPass*> configure(
            const resolution& resolution,
            const string& resourcesPath,
            meshRendererDescriptor* renderDescriptor,
            framebufferAllocator* framebufferAllocator);
    };
}