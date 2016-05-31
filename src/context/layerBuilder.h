#pragma once
#include <phi.h>

#include <rendering\gl.h>
#include <rendering\program.h>

#include "layer.h"

namespace demon
{
    class layerBuilder
    {
    private:
        static phi::program* buildControlRenderPassProgram(const phi::string& shadersPath);
        static phi::program* buildTextRenderPassProgram(const phi::string& shadersPath);
        static phi::program* buildGBufferRenderPassProgram(const phi::string& shadersPath);
        static phi::program* buildLightingRenderPassProgram(const phi::string& shadersPath);

    public:
        static layer* buildScene(const phi::string& resourcesPath, phi::gl* gl, float width, float height);
        static layer* buildUI(const phi::string& resourcesPath, phi::gl* gl, float width, float height);
    };
}