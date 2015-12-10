#ifndef _PHI_TEXT_RENDERER_H_
#define _PHI_TEXT_RENDERER_H_

#include <phi/core/size.h>

#include "rendering.h"
#include "shaderManager.h"
#include "shader.h"
#include "quad.h"
#include "font.h"

namespace phi
{
    class textRenderer2D
    {
    private:
        quad* _quad;
        geometry* _quadGeometry;
        shader* _shader;
        sizef _viewportSize;
        mat4 _viewMatrix;
        mat4 _projectionMatrix;
        mat4 _modelMatrix;
    private:
        void updateViewMatrix();
        void updateProjMatrix();
    public:
        RENDERING_API textRenderer2D(sizef viewportSize);
        RENDERING_API ~textRenderer2D();
        RENDERING_API void update();
        RENDERING_API void setViewportSize(sizef viewportSize) { _viewportSize = viewportSize; }
        RENDERING_API void render(std::string text, font* font, color foreColor, color backColor, vec2 location, float zIndex);
        RENDERING_API sizef measureSize(std::string text, font* font);
        RENDERING_API unsigned int measureString(std::string text, font* font, sizef size);
    };
}

#endif