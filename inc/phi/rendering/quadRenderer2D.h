#ifndef _PHI_QUAD_RENDERER_H_
#define _PHI_QUAD_RENDERER_H_

#include <phi/core/size.h>

#include "rendering.h"
#include "shaderManager.h"
#include "shader.h"
#include "quad.h"
#include "transform.h"

namespace phi
{
    class quadRenderer2D
    {
    private:
        quad _quad;
        transform* _transform;
        shader* _shader;
        size<GLuint> _viewportSize;
        size<GLuint> _size;
        glm::vec2 _location;
        float _zIndex;
    private:
        void updateViewMatrix();
        void updateProjMatrix();
        void updateModelMatrix();
    public:
		RENDERING_API quadRenderer2D(glm::vec2 location, float zIndex, size<GLuint> quadSize, size<GLuint> viewportSize);
        RENDERING_API ~quadRenderer2D();
        RENDERING_API void update();
        RENDERING_API glm::vec2 getLocation() { return _location; }
        RENDERING_API float getZIndex() { return _zIndex; }
		RENDERING_API size<GLuint> getSize() { return _size; }
        RENDERING_API void setLocation(glm::vec2 location) { _location = location; }
        RENDERING_API void setZIndex(float value) { _zIndex = value; }
		RENDERING_API void setSize(size<GLuint> size) { _size = size; }
        RENDERING_API void setViewportSize(size<GLuint> viewportSize) { _viewportSize = viewportSize; }
        RENDERING_API void render(texture* image, color backColor);
    };
}

#endif