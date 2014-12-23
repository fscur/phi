#include "globals.h"
#include "textRenderer2D.h"
#include "meshRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"

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

	textRenderer2D::textRenderer2D(size<GLuint> viewportSize)
    {
        _viewportSize = viewportSize;

        updateViewMatrix();
        updateProjMatrix();

        _shader = shaderManager::get()->getShader("HUD_TEXT");
    }

    textRenderer2D::~textRenderer2D()
    {}

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
		_projectionMatrix = glm::ortho<float>(0.0f, (float)_viewportSize.width, 0.0f, (float)_viewportSize.height, -50.0f, 50.0f);
    }

    //TODO:Cache generated text
    void textRenderer2D::render(std::string text, font* font, color foreColor, color backColor, glm::vec2 location)
    {
        if (text.empty())
            return;

        SDL_Surface *initial;
        SDL_Surface *intermediary;
        int w,h;

        SDL_Color sdlForeColor = { 
            static_cast<Uint8>(foreColor.r * 255),
            static_cast<Uint8>(foreColor.g * 255),
            static_cast<Uint8>(foreColor.b * 255),
            static_cast<Uint8>(foreColor.a * 255)
        };

        if (backColor != color::transparent)
        {
            SDL_Color sdlBackColor = { 
                static_cast<Uint8>(backColor.r * 255),
                static_cast<Uint8>(backColor.g * 255),
                static_cast<Uint8>(backColor.b * 255),
                static_cast<Uint8>(backColor.a * 255)
            };

            initial = TTF_RenderText_Shaded(font->getTtfFont(), text.c_str(), sdlForeColor, sdlBackColor);
        }
        else
            initial = TTF_RenderText_Blended(font->getTtfFont(), text.c_str(), sdlForeColor);

        w = nextPowerOfTwo(initial->w);
        h = nextPowerOfTwo(initial->h);

        intermediary = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
		
        SDL_BlitSurface(initial, 0, intermediary, 0);
		
		SDL_InvertSurface(intermediary);

        /*glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, intermediary->pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); */

		texture* textTexture = texture::create(size<GLuint>(w, h), GL_RGBA, GL_BGRA, GL_UNSIGNED_BYTE, 0, intermediary->pixels);
		textTexture->setParam(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		textTexture->setParam(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		textTexture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		textTexture->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        _modelMatrix = glm::mat4(
            w, 0.0f, 0.0f, 0.0f,
            0.0f, h, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
			location.x + w * 0.5f, _viewportSize.height - location.y - h * 0.5f, 0.1f, 1.0f);

        _shader->bind();
		_shader->setUniform("mvp", _projectionMatrix * _viewMatrix * _modelMatrix);
		_shader->setUniform("textTexture", textTexture);
        meshRenderer::render(&_quad);
        _shader->unbind();

        SDL_FreeSurface(initial);
        SDL_FreeSurface(intermediary);
		textTexture->release();
		DELETE(textTexture);
        //glDisable(GL_BLEND);
    }

    size<int> textRenderer2D::measureSize(std::string text, font* font)
    {
        if (text.empty())
            return size<int>(0, 0);

        SDL_Surface *initial;
        int w,h;

        SDL_Color color = { 
            static_cast<Uint8>(255),
            static_cast<Uint8>(255),
            static_cast<Uint8>(255),
            static_cast<Uint8>(255)
        };

        initial = TTF_RenderText_Solid(font->getTtfFont(), text.c_str(), color);

        w = nextPowerOfTwo(initial->w);
        h = nextPowerOfTwo(initial->h);

		SDL_FreeSurface(initial);

        return size<int>(initial->w, initial->h);
    }
}