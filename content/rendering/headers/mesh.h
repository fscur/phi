#ifndef _PHI_MESH_H_
#define _PHI_MESH_H_

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include <string>
#include <list>
#include <vector>
#include "vertex.h"
#include "resource.h"
#include "material.h"

namespace phi
{
	class mesh :
        public resource
	{
	private:
		GLuint _vao;
		GLuint _verticesVbo;
		GLuint _texCoordsVbo;
		GLuint _normalsVbo;
		GLuint _tangentsVbo;		
		GLuint _indicesVbo;

		GLuint _indicesCount;

        GLfloat* _positionsBuffer;
	    GLfloat* _texCoordsBuffer; 
        GLfloat* _normalsBuffer;
        GLfloat* _tangentsBuffer;
        GLuint* _indicesBuffer;

        GLuint _pSize;
        GLuint _tSize;
        GLuint _nSize;
        GLuint _tgSize;
        GLuint _iSize;

		std::vector<vertex> _vertices;
		std::vector<GLuint> _indices;
		bool _isBound;

        std::string _materialName;
        material* _material;

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
		RENDERING_API mesh(std::string name, std::string path);
		RENDERING_API ~mesh();

		RENDERING_API static mesh* create(std::string name, std::vector<vertex> &vertices, std::vector<GLuint> &indices);
		RENDERING_API static std::string getToken(std::string line);

		RENDERING_API bool getIsBound() const { return _isBound; }
        RENDERING_API std::vector<vertex> getVertices() const { return _vertices; }
		RENDERING_API std::vector<GLuint> getIndices() const { return _indices; }
        RENDERING_API material* getMaterial() const { return _material; }
        RENDERING_API std::string getMaterialName() const { return _materialName; }

        RENDERING_API void setMaterial(material* value);

		RENDERING_API static void calcNormals(std::vector<vertex> &vertices, std::vector<GLuint> indices);
		RENDERING_API static void calcTangents(std::vector<vertex> &vertices, std::vector<GLuint> indices);

		RENDERING_API void bind();
		RENDERING_API void render();
		RENDERING_API void unbind();

		RENDERING_API void saveToModel(std::string fileName);

		RENDERING_API static mesh* fromMesh(const std::string fileName);
		RENDERING_API static mesh* fromObj(std::string name, const std::string fileName, bool calcNormals = false);
	};
}
#endif