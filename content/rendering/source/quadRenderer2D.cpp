#include "quadRenderer2D.h"
#include "meshRenderer.h"
#include <glm/gtc/matrix_transform.hpp>

namespace phi
{
	quadRenderer2D::quadRenderer2D(glm::vec2 location, float zIndex, size<GLuint> quadSize, size<GLuint> viewportSize)
	{
		_location = location;
        _zIndex = zIndex;
		_size = quadSize;
		_viewportSize = viewportSize;
		_shader = shaderManager::get()->getShader("HUD_QUAD");

		_transform = new transform();

		update();
	}

	quadRenderer2D::~quadRenderer2D()
	{
		delete _transform;
	}

	void quadRenderer2D::update()
	{
		updateViewMatrix();
		updateProjMatrix();
		updateModelMatrix();
	}

	void quadRenderer2D::updateViewMatrix()
	{
		_transform->setViewMatrix(glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	void quadRenderer2D::updateProjMatrix()
	{
		_transform->setProjectionMatrix(glm::ortho<float>(0.0f, (float)_viewportSize.width, 0.0f, (float)_viewportSize.height, -50.0f, 50.0f));
	}

	void quadRenderer2D::updateModelMatrix()
	{
		int w = _size.width;
		int h = _size.height;

		_transform->setModelMatrix(
			glm::mat4(
			w, 0.0f, 0.0f, 0.0f,
			0.0f, h, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
            _location.x + w * 0.5f, _viewportSize.height - _location.y - h * 0.5f, _zIndex, 1.0f));
	}

	void quadRenderer2D::render(texture* image, color backColor)
	{
		_shader->bind();
		_shader->setUniform("mvp", _transform->getMvp());
		_shader->setUniform("quadTexture", image);
		_shader->setUniform("backColor", backColor);
		meshRenderer::render(&_quad);
		_shader->unbind();
	}
}