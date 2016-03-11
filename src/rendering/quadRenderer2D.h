#pragma once
#include <phi.h>
#include "rendering.h"

#include <core\size.h>

#include "shadersManager.h"
#include "shader.h"
//#include "quad.h"
//#include "transform.h"

namespace phi
{
    class quadRenderer2D
    {
    private:
        //quad _quad;
        //transform* _transform;
        shader* _shader;
        sizeui _viewportSize;
        sizeui _size;
        glm::vec2 _location;
        float _zIndex;
    private:
        void updateViewMatrix();
        void updateProjMatrix();
        void updateModelMatrix();
    public:
        RENDERING_API quadRenderer2D(vec2 location, float zIndex, sizeui quadSize, sizeui viewportSize);
        RENDERING_API ~quadRenderer2D();
        RENDERING_API void update();
        RENDERING_API void render(texture* image, color backColor);

        glm::vec2 getLocation() { return _location; }
        float getZIndex() { return _zIndex; }
        sizeui getSize() { return _size; }
        void setLocation(glm::vec2 location) { _location = location; }
        void setZIndex(float value) { _zIndex = value; }
        void setSize(sizeui size) { _size = size; }
        void setViewportSize(sizeui viewportSize) { _viewportSize = viewportSize; }
    };
}