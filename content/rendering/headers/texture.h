#ifndef _PHI_TEXTURE_H_
#define _PHI_TEXTURE_H_

#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif
#include "rendering.h"
#include <string>
#include <vector>
#include "size.h"
#include "SDL_Extensions.h"
#include "resource.h"

namespace phi
{
    class texture :
        public resource
    {
    private:
        GLuint _id;
        GLuint _textureType;

	protected:
		size<GLuint> _size;

    private:
		texture(GLuint id, size<GLuint> size, std::string name, std::string path);

    public:
        RENDERING_API ~texture();

		RENDERING_API GLuint getId() const { return _id; }
		RENDERING_API size<GLuint> getSize() const { return _size; }
        RENDERING_API GLuint getTextureType() const { return _textureType; }

		RENDERING_API void bind(GLuint level = 0);
		RENDERING_API void setParam(GLenum name, GLint value);
        RENDERING_API void release();

		RENDERING_API static texture* fromFile(std::string fileName);
		RENDERING_API static texture* create(size<GLuint> size, GLint internalFormat = GL_RGB32F, GLint format = GL_RGBA, GLint type = GL_FLOAT, GLuint level = 0, GLvoid* data = 0);
        RENDERING_API static texture* createCubeMap(size<GLuint> size, GLint internalFormat = GL_RGB32F, GLint format = GL_RGBA, GLint type = GL_FLOAT, GLuint level = 0, const std::vector<GLvoid*> data = std::vector<GLvoid*>());
    };
}
#endif