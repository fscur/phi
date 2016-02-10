#include "textRenderer2D.h"

#include "texture.h"

#include <core\globals.h>

#include <glm\gtc\matrix_transform.hpp>

namespace phi
{
    int round(double x)
    {
        return (int)(x + 0.5);
    }

    int nextPowerOfTwo(int x)
    {
        double logbase2 = std::log(x) / std::log(2);
        return round(pow(2,ceil(logbase2)));
    }

    textRenderer2D::textRenderer2D(sizef viewportSize)
    {
        _viewportSize = viewportSize;

        updateViewMatrix();
        updateProjMatrix();
        _modelMatrix = mat4(1.0f);

        //_shader = shaderManager::get()->getShader("HUD_TEXT");
        _quad = geometry::quad();
    }

    textRenderer2D::~textRenderer2D()
    {
    }

    void textRenderer2D::update()
    {
        updateViewMatrix();
        updateProjMatrix();
    }

    void textRenderer2D::updateViewMatrix()
    {
        _viewMatrix = lookAt(vec3(0.0f, 0.0f, 1.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
    }

    void textRenderer2D::updateProjMatrix()
    {
        _projectionMatrix = glm::ortho<float>(0.0f, (float)_viewportSize.w, 0.0f, (float)_viewportSize.h, -50.0f, 50.0f);
    }

    void textRenderer2D::render(std::string text, font* font, color foreColor, color backColor, vec2 location, float zIndex)
    {
        //_shader->bind();

        //_shader->setUniform("res", vec2(_viewportSize.w, _viewportSize.h));
        //_shader->setUniform("color", foreColor);
        //_shader->setUniform("texture", font->getTexture(), 0);
        //_shader->setUniform("texSize", vec2(font->getTexWidth(), font->getTexHeight()));

        //mat4 vp = _projectionMatrix * _viewMatrix;

        //float currentX = location.x, currentY = location.y;
        //for(char& p : text)
        //{
        //    if (p == '\n')
        //    {
        //        currentX = location.x;
        //        currentY += font->getLineHeight();
        //        continue;
        //    }

        //    float x = currentX + font->c[p].bl;
        //    float y = currentY + font->getAscender() - font->c[p].bt;// - (font->c[p].bh - font->c[p].bt - font->getBaseLine()) * sy;
        //    float w = font->c[p].bw;
        //    float h = font->c[p].bh;

        //    currentX += font->c[p].ax;
        //    currentY += font->c[p].ay;

        //    float tl = font->c[p].tx;
        //    float tr = font->c[p].tx + font->c[p].bw / font->getTexWidth();
        //    float tt = font->c[p].ty;
        //    float tb = font->c[p].ty + font->c[p].bh / font->getTexHeight();

        //    _modelMatrix = mat4(
        //        w, 0.0f, 0.0f, 0.0f,
        //        0.0f, h, 0.0f, 0.0f,
        //        0.0f, 0.0f, 1.0f, 0.0f,
        //        x + w * 0.5f, _viewportSize.h - (y + h * 0.5f), zIndex, 1.0f);

        //    _shader->setUniform("mvp", vp * _modelMatrix);
        //    _shader->setUniform("texCoordOrigin", vec2(tl, tt));
        //    _shader->setUniform("texCoordQuadSize", vec2(font->c[p].bw / font->getTexWidth(), font->c[p].bh / font->getTexHeight()));

        //    geometryRenderer::render(_quadGeometry);
        //}

        //_shader->unbind();
    }

    sizef textRenderer2D::measureSize(std::string text, font* font)
    {
        if (text.empty())
            return sizef(0, 0);

        float maxWidth = 0.0f;
        float currentWidth = 0.0f;
        float currentHeight = (float)font->getLineHeight();

        for(char& p : text)
        {
            if (p == '\n')
            {
                currentWidth = 0.0f;
                currentHeight += font->getLineHeight();
                continue;
            }

            currentWidth += font->c[p].ax;
            currentHeight += font->c[p].ay;
            maxWidth = glm::max(currentWidth, maxWidth);
        }

        return sizef(maxWidth, currentHeight);
    }

    unsigned int textRenderer2D::measureString(std::string text, font* font, sizef sz)
    {
        if (text.empty())
            return 0;

        unsigned int index = 0;
        float currentWidth = 0.0f;
        for(char& p : text)
        {
            if (p == '\n')
            {
                currentWidth = 0.0f;
                continue;
            }

            if (currentWidth + font->c[p].ax > sz.w)
                break;

            currentWidth += font->c[p].ax;
            index++;
        }

        return index;
    }
}