#ifndef _PHI_RENDER_TARGET_H_
#define _PHI_RENDER_TARGET_H_

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include "rendering.h"
#include "globals.h"
#include "color.h"
#include "size.h"

namespace phi
{
	class renderTarget
	{
	protected:
		bool _isInitialized;
		GLuint _id;
		GLuint _x;
		GLuint _y;
		size<GLuint> _size;
		bool _isBound;
		color _clearColor;
	protected:
		renderTarget();
	public:
		RENDERING_API renderTarget(size<GLuint> size, color backColor);
		RENDERING_API virtual ~renderTarget();

		RENDERING_API GLuint getId() const { return _id; }
		RENDERING_API size<GLuint> getSize() const { return _size; }   
		RENDERING_API color getClearColor() const { return _clearColor; }
		RENDERING_API bool getIsBound() const { return _isBound; }

		RENDERING_API void setSize(size<GLuint> value);
		RENDERING_API void setClearColor(color value);

		RENDERING_API virtual bool init();

		RENDERING_API void bind();
		RENDERING_API void bindForWriting();
		RENDERING_API void bindForReading();

		RENDERING_API void setViewport(GLuint x, GLuint y, size<GLuint> size);
		RENDERING_API virtual void clear();
		RENDERING_API void unbind();
	};
}

#endif