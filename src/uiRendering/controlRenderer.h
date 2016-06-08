#pragma once
#include <phi.h>
#include "uiRenderingApi.h"

#include <core\resolution.h>

#include <rendering\gl.h>
#include <rendering\renderPass.h>

#include "controlRendererDescriptor.h"

namespace phi
{
    class controlRenderer
    {
    private:
        vector<renderPass*> _renderPasses;

    private:
        UI_RENDERING_API controlRenderer(vector<renderPass*>&& renderPasses);

    public:
        UI_RENDERING_API  ~controlRenderer();
        UI_RENDERING_API  void render();

        UI_RENDERING_API static vector<renderPass*> configure(gl* gl, const string& resourcesPath, controlRendererDescriptor* renderDescriptor);
    };
}