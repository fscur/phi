#ifndef _PHI_QUAD_RENDERER_H_
#define _PHI_QUAD_RENDERER_H_

#include <phi\core\size.h>
#include <phi\core\globals.h>
#include <phi\core\geometry.h>

#include "rendering.h"
#include "shaderManager.h"
#include "shader.h"
#include "transform.h"

namespace phi
{
    class quadRenderer2D
    {
    private:
        geometry* _quad;
        transform* _transform;
        shader* _shader;
        sizef _viewportSize;
        sizef _size;
        vec2 _location;
        float _zIndex;
    private:
        void updateViewMatrix();
        void updateProjMatrix();
        void updateModelMatrix();
    public:
		RENDERING_API quadRenderer2D(vec2 location, float zIndex, sizef quadSize, sizef viewportSize);
        RENDERING_API ~quadRenderer2D();
        RENDERING_API void update();
        RENDERING_API vec2 getLocation() { return _location; }
        RENDERING_API float getZIndex() { return _zIndex; }
		RENDERING_API sizef getSize() { return _size; }
        RENDERING_API void setLocation(vec2 location) { _location = location; }
        RENDERING_API void setZIndex(float value) { _zIndex = value; }
		RENDERING_API void setSize(sizef size) { _size = size; }
        RENDERING_API void setViewportSize(sizef viewportSize) { _viewportSize = viewportSize; }
        RENDERING_API void render(texture* image, color backColor);
    };
}

#endif