#include <precompiled.h>
#include "textRenderer2D.h"

//#include <phi/rendering/meshRenderer.h>
#include "texture.h"
#include <glm/gtc/matrix_transform.hpp>

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

    textRenderer2D::textRenderer2D(sizeui viewportSize)
    {
        _viewportSize = viewportSize;

        updateViewMatrix();
        updateProjMatrix();
        _modelMatrix = glm::mat4(1.0f);

        //_shader = shadersManager::get()->getShader("HUD_TEXT");

        std::vector<vertex> vertices;
        vertices.push_back(vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 1.0f)));
        vertices.push_back(vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 1.0f)));
        vertices.push_back(vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 0.0f)));
        vertices.push_back(vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 0.0f)));

        auto indices = new std::vector<GLuint>();
        indices->push_back(0);
        indices->push_back(1);
        indices->push_back(2);
        indices->push_back(2);
        indices->push_back(3);
        indices->push_back(0);

        //_mesh = mesh::create("", vertices, indices);
    }

    textRenderer2D::~textRenderer2D()
    {
        safeDelete(_mesh);
    }

    void textRenderer2D::update()
    {
        updateViewMatrix();
        updateProjMatrix();
    }

    void textRenderer2D::updateViewMatrix()
    {
        _viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void textRenderer2D::updateProjMatrix()
    {
        _projectionMatrix = glm::ortho<float>(0.0f, (float)_viewportSize.w, 0.0f, (float)_viewportSize.h, -50.0f, 50.0f);
    }

    void textRenderer2D::render(std::string text, font* font, color foreColor, color backColor, glm::vec2 location, float zIndex)
    {
        //_shader->bind();

        //_shader->setUniform("res", glm::vec2(_viewportSize.w, _viewportSize.h));
        //_shader->setUniform("color", foreColor);
        //_shader->setUniform("texture", font->getTexture(), 0);
        //_shader->setUniform("texSize", glm::vec2(font->getTexWidth(), font->getTexHeight()));

        //meshRenderer mr = meshRenderer();

        glm::mat4 vp = _projectionMatrix * _viewMatrix;

        float currentX = location.x, currentY = location.y;
        for(char& p : text)
        {
            if (p == '\n')
            {
                currentX = location.x;
                currentY += font->getLineHeight();
                continue;
            }

            float x = currentX + font->c[p].bl;
            float y = currentY + font->getAscender() - font->c[p].bt;// - (font->c[p].bh - font->c[p].bt - font->getBaseLine()) * sy;
            float w = font->c[p].bw;
            float h = font->c[p].bh;

            currentX += font->c[p].ax;
            currentY += font->c[p].ay;

            float tl = font->c[p].tx;
            float tr = font->c[p].tx + font->c[p].bw / font->getTexWidth();
            float tt = font->c[p].ty;
            float tb = font->c[p].ty + font->c[p].bh / font->getTexHeight();

            /*_modelMatrix = glm::mat4(
                w, 0.0f, 0.0f, 0.0f,
                0.0f, h, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                x + w * 0.5f, _viewportSize.height - (y + h * 0.5f), zIndex, 1.0f);

            _shader->setUniform("mvp", vp * _modelMatrix);
            _shader->setUniform("texCoordOrigin", glm::vec2(tl, tt));
            _shader->setUniform("texCoordQuadSize", glm::vec2(font->c[p].bw / font->getTexWidth(), font->c[p].bh / font->getTexHeight()));

            mr.render(_mesh);*/
        }

        _shader->unbind();
    }

    sizef textRenderer2D::measureSize(std::string text, font* font)
    {
        if (text.empty())
            return sizef(0.0f, 0.0f);

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

    unsigned int textRenderer2D::measureString(std::string text, font* font, sizeui sz)
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