#ifndef _PHI_FRAME_BUFFER_H_
#define _PHI_FRAME_BUFFER_H_

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

#include <glm/glm.hpp>

#include <map>
#include "rendering.h"
#include "globals.h"
#include "color.h"
#include "size.h"
#include "renderTarget.h"

namespace phi
{
	class frameBuffer
	{
	protected:
		std::string _name;
		bool _isInitialized;
		GLuint _id;
		GLuint _x;
		GLuint _y;
		size<GLuint> _size;
		bool _isBound;
		color _clearColor;
		std::map<std::string, renderTarget*>* _renderTargets;
	
	public:
		RENDERING_API frameBuffer(std::string name, size<GLuint> size, color backColor);
		RENDERING_API virtual ~frameBuffer();

		RENDERING_API GLuint getId() const { return _id; }
		RENDERING_API size<GLuint> getSize() const { return _size; }   
		RENDERING_API color getClearColor() const { return _clearColor; }
		RENDERING_API bool getIsBound() const { return _isBound; }

		RENDERING_API void setSize(size<GLuint> value);
		RENDERING_API void setClearColor(color value);

		RENDERING_API virtual void init();

		RENDERING_API bool isComplete();

		RENDERING_API void bind();
		RENDERING_API void bindForDrawing();
		RENDERING_API void bindForReading();
		RENDERING_API void unbind();

		RENDERING_API void enable(GLenum value);

		RENDERING_API void setViewport(GLuint x, GLuint y, size<GLuint> size);
		RENDERING_API virtual void clear();

		RENDERING_API void blit(std::string renderTargetName, GLuint x, GLuint y, GLsizei width, GLsizei height);

		RENDERING_API bool addRenderTarget(renderTarget* renderTarget);
		RENDERING_API renderTarget* getRenderTarget(std::string name);

		RENDERING_API renderTarget* newRenderTarget(
			std::string name,
			texture* texture,
			GLenum _target = GL_DRAW_FRAMEBUFFER,
			GLenum _attachment = GL_COLOR_ATTACHMENT0,
			GLenum _texTarget = GL_TEXTURE_2D,
			GLuint _level = 0);

		RENDERING_API GLfloat getZBufferValue(glm::vec2 mousePos);
	};
}

#endif