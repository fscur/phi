#ifndef _PHI_MESH_H_
#define _PHI_MESH_H_

#include "vertex.h"

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include <string>
#include <list>
#include <vector>

namespace phi
{
	class mesh
	{
	private:
		GLuint _vao;
		GLuint _verticesVbo;
		GLuint _texCoordsVbo;
		GLuint _normalsVbo;
		GLuint _tangentsVbo;		
		GLuint _indicesVbo;

		GLuint _indicesCount;

		std::vector<vertex> _vertices;
		std::vector<GLuint> _indices;
		bool _isBound;

	private:
		static void createBuffers(
			std::vector<vertex> vertices, 
			GLfloat* &vertexBuffer, 
			GLfloat* &texCoordBuffer, 
			GLfloat* &normalBuffer, 
			GLfloat* &tangentBuffer);

	protected:

	public:
		
		RENDERING_API void addVertices(std::vector<vertex> vertices, std::vector<GLuint> indices);
		RENDERING_API mesh();
		RENDERING_API ~mesh();

		RENDERING_API static mesh* create(std::vector<vertex> &vertices, std::vector<GLuint> &indices);
		RENDERING_API static std::string getToken(std::string line);

		RENDERING_API bool getIsBound() const { return _isBound; }

		RENDERING_API std::vector<vertex> getVertices() const { return _vertices; }
		RENDERING_API std::vector<GLuint> getIndices() const { return _indices; }

		RENDERING_API static void calcNormals(std::vector<vertex> &vertices, std::vector<GLuint> indices);
		RENDERING_API static void calcTangents(std::vector<vertex> &vertices, std::vector<GLuint> indices);

		RENDERING_API void bind();
		RENDERING_API void render();
		RENDERING_API void unbind();

		RENDERING_API void saveToMb(std::string fileName);

		RENDERING_API static mesh* fromMb(const std::string fileName);
		RENDERING_API static mesh* fromObj(const std::string fileName, bool calcNormals = false);
	};
}
#endif