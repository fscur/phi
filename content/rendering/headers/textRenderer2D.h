#ifndef _PHI_TEXT_RENDERER_H_
#define _PHI_TEXT_RENDERER_H_

#include "rendering.h"

#include "shaderManager.h"
#include "shader.h"
#include "quad.h"
#include "font.h"
#include "size.h"

namespace phi
{
    class textRenderer2D
    {
    private:
        mesh* _mesh;
        shader* _shader;
		size<GLuint> _viewportSize;
		glm::mat4 _viewMatrix;
		glm::mat4 _projectionMatrix;
		glm::mat4 _modelMatrix;
    private:
        void updateViewMatrix();
        void updateProjMatrix();
    public:
		RENDERING_API textRenderer2D(size<GLuint> viewportSize);
        RENDERING_API ~textRenderer2D();
        RENDERING_API void update();
		RENDERING_API void setViewportSize(size<GLuint> viewportSize) { _viewportSize = viewportSize; }
        RENDERING_API void render(std::string text, font* font, color foreColor, color backColor, glm::vec2 location, float zIndex);
        RENDERING_API size<int> measureSize(std::string text, font* font);
    };
}

#endif