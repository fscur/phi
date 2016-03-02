#include <precompiled.h>
#include "quadRenderer2D.h"

namespace phi
{
    quadRenderer2D::quadRenderer2D(vec2 location, float zIndex, sizeui quadSize, sizeui viewportSize)
    {
        _location = location;
        _zIndex = zIndex;
        _size = quadSize;
        _viewportSize = viewportSize;
        //_transform = new transform();
        //_quad = geometry::quad();
        update();
    }

    quadRenderer2D::~quadRenderer2D()
    {
        //safeDelete(_transform);
    }

    void quadRenderer2D::update()
    {
        updateViewMatrix();
        updateProjMatrix();
        updateModelMatrix();
    }

    void quadRenderer2D::updateViewMatrix()
    {
        //_transform->setViewMatrix(lookAt(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f)));
    }

    void quadRenderer2D::updateProjMatrix()
    {
        //_transform->setProjectionMatrix(glm::ortho<float>(0.0f, (float)_viewportSize.w, 0.0f, (float)_viewportSize.h, -50.0f, 50.0f));
    }

    void quadRenderer2D::updateModelMatrix()
    {
        int w = (int)_size.w;
        int h = (int)_size.h;

        //_transform->setModelMatrix(
        //    mat4(
        //    w, 0.0f, 0.0f, 0.0f,
        //    0.0f, h, 0.0f, 0.0f,
        //    0.0f, 0.0f, 1.0f, 0.0f,
        //    _location.x + w * 0.5f, _viewportSize.h - _location.y - h * 0.5f, _zIndex, 1.0f));
    }

    void quadRenderer2D::render(texture* image, color backColor)
    {
        //_shader->bind();
        //_shader->setUniform("mvp", _transform->getMvp());
        //_shader->setUniform("quadTexture", image, 0);
        //_shader->setUniform("backColor", backColor);
        //geometryRenderer::render(_quadGeometry);
        //_shader->unbind();
    }
}