#ifndef _PHI_CUBE_MAP_H_
#define _PHI_CUBE_MAP_H_

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
    class cubeMap :
        public resource
    {
    private:
        GLuint _id;
        GLuint _textureType;

	protected:
		size<GLuint> _size;

    private:
		cubeMap(GLuint id, size<GLuint> size, std::string name, std::string path);

    public:
        RENDERING_API ~cubeMap();

		RENDERING_API GLuint getId() const { return _id; }
		RENDERING_API size<GLuint> getSize() const { return _size; }

		RENDERING_API void bind(GLuint level = 0);
		RENDERING_API void setParam(GLenum name, GLint value);
        RENDERING_API void release();

		RENDERING_API static cubeMap* fromFile(std::string fileName);
		RENDERING_API static cubeMap* createCubeMap(size<GLuint> size, GLint internalFormat = GL_RGB32F, GLint format = GL_RGBA, GLint type = GL_FLOAT, GLuint level = 0, const std::vector<GLvoid*> data = std::vector<GLvoid*>());
    };
}
#endif