#pragma once
#include <phi.h>
#include "rendering.h"

#include <core\size.h>

#include "shadersManager.h"
#include "shader.h"
//#include "quad.h"
#include "font.h"
#include "mesh.h"

namespace phi
{
    class textRenderer2D
    {
    private:
        mesh* _mesh;
        shader* _shader;
        sizeui _viewportSize;
        glm::mat4 _viewMatrix;
        glm::mat4 _projectionMatrix;
        glm::mat4 _modelMatrix;
    private:
        void updateViewMatrix();
        void updateProjMatrix();
    public:
        RENDERING_API textRenderer2D(sizeui viewportSize);
        RENDERING_API ~textRenderer2D();
        RENDERING_API void update();
        RENDERING_API void setViewportSize(sizeui viewportSize) { _viewportSize = viewportSize; }
        RENDERING_API void render(std::string text, font* font, color foreColor, color backColor, glm::vec2 location, float zIndex);
        RENDERING_API sizef measureSize(std::string text, font* font);
        RENDERING_API unsigned int measureString(std::string text, font* font, sizeui size);
    };
}