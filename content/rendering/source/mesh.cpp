#include "mesh.h"
#include "globals.h"

#include <fstream>
#include <iostream>
#include <vector>

namespace phi
{
	mesh::mesh()
	{
		_vao = 0;

		_verticesVbo = 0;
		_texCoordsVbo = 0;
		_normalsVbo = 0;
		_tangentsVbo = 0;
		_indicesVbo = 0;
		_indicesCount = 0;

		_isBound = false;
	}

	mesh::~mesh()
	{   
		glDeleteBuffers(1, &_verticesVbo);
		glDeleteBuffers(1, &_texCoordsVbo);
		glDeleteBuffers(1, &_normalsVbo);
		glDeleteBuffers(1, &_tangentsVbo);
		glDeleteBuffers(1, &_indicesVbo);
		glDeleteVertexArrays(1, &_vao);
	}

	mesh* mesh::create(std::vector<vertex> &vertices, std::vector<GLuint> &indices)
	{
		mesh* m = new mesh();
		m->addVertices(vertices, indices);
		return m;
	}

	void mesh::addVertices(std::vector<vertex> vertices, std::vector<GLuint> indices)
	{
		_vertices = vertices;
		_indices = indices;
		_indicesCount = indices.size();

		GLuint verticesSize = vertices.size() * 3 * sizeof(GLfloat);
		GLuint texCoordsSize = vertices.size() * 2 * sizeof(GLfloat);
		GLuint normalsSize = vertices.size() * 3 * sizeof(GLfloat);
		GLuint tangentsSize = vertices.size() * 3 * sizeof(GLfloat);
		GLuint indicesSize = _indicesCount * sizeof(GLuint);

		GLfloat* vertexBuffer = new GLfloat[vertices.size() * 3];
		GLfloat* texCoordBuffer = new GLfloat[vertices.size() * 2];
		GLfloat* normalBuffer = new GLfloat[vertices.size() * 3];
		GLfloat* tangentBuffer = new GLfloat[vertices.size() * 3];

		createBuffers(vertices, vertexBuffer, texCoordBuffer, normalBuffer, tangentBuffer);

		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glGenBuffers(1, &_verticesVbo);
		glBindBuffer(GL_ARRAY_BUFFER, _verticesVbo);
		glBufferData(GL_ARRAY_BUFFER, verticesSize, vertexBuffer, GL_STATIC_DRAW);

		glGenBuffers(1, &_texCoordsVbo);
		glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
		glBufferData(GL_ARRAY_BUFFER, texCoordsSize, texCoordBuffer, GL_STATIC_DRAW);

		glGenBuffers(1, &_normalsVbo);
		glBindBuffer(GL_ARRAY_BUFFER, _normalsVbo);
		glBufferData(GL_ARRAY_BUFFER, normalsSize, normalBuffer, GL_STATIC_DRAW);

		glGenBuffers(1, &_tangentsVbo);
		glBindBuffer(GL_ARRAY_BUFFER, _tangentsVbo);
		glBufferData(GL_ARRAY_BUFFER, tangentsSize, tangentBuffer, GL_STATIC_DRAW);

		glGenBuffers(1, &_indicesVbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, &indices[0], GL_STATIC_DRAW);

		delete[] vertexBuffer;
		delete[] texCoordBuffer;
		delete[] normalBuffer;
		delete[] tangentBuffer;
	}

	void mesh::createBuffers(std::vector<vertex> vertices, GLfloat* &vertexBuffer, GLfloat* &texCoordBuffer, GLfloat* &normalBuffer, GLfloat* &tangentBuffer)
	{
		std::vector<vertex>::iterator i;
		unsigned int vIndex = 0;
		unsigned int tIndex = 0;
		unsigned int nIndex = 0;
		unsigned int tgIndex = 0;

		for (i = vertices.begin(); i != vertices.end(); ++i)
		{
			vertex vertex = *i;

			GLfloat x = vertex.getPosition().x;
			GLfloat y = vertex.getPosition().y;
			GLfloat z = vertex.getPosition().z;

			GLfloat u = vertex.getTexCoord().x;
			GLfloat v = vertex.getTexCoord().y;

			GLfloat r0 = vertex.getNormal().x;
			GLfloat s0 = vertex.getNormal().y;
			GLfloat t0 = vertex.getNormal().z;

			GLfloat r1 = vertex.getTangent().x;
			GLfloat s1 = vertex.getTangent().y;
			GLfloat t1 = vertex.getTangent().z;

			vertexBuffer[vIndex++] = x;
			vertexBuffer[vIndex++] = y;
			vertexBuffer[vIndex++] = z;

			texCoordBuffer[tIndex++] = u;
			texCoordBuffer[tIndex++] = v;

			normalBuffer[nIndex++] = r0;
			normalBuffer[nIndex++] = s0;
			normalBuffer[nIndex++] = t0;

			tangentBuffer[tgIndex++] = r1;
			tangentBuffer[tgIndex++] = s1;
			tangentBuffer[tgIndex++] = t1;
		}
	}

	void mesh::bind()
	{
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _verticesVbo);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
		glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, _normalsVbo);
		glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, _tangentsVbo);
		glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
		_isBound = true;
	}

	void mesh::render()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
		glDrawElements(GL_TRIANGLES, _indicesCount, GL_UNSIGNED_INT, 0);
	}

	void mesh::unbind()
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		_isBound = false;
	}

	std::string mesh::getToken(std::string line)
	{
		std::string token = "";

		if (line.size() == 0)
			return token;

		unsigned int i = 0;

		do
		{
			token += line[i];
		}
		while(++i<line.size() && line[i] != ' ');

		return token;
	}

	void mesh::saveToMb(std::string fileName)
	{
		LOG("SaveToMb: Loading " << fileName);

		GLuint vSize = sizeof (vertex) * _vertices.size();
		GLuint iSize = sizeof (GLuint) * _indices.size();

		std::ofstream oFile;
		oFile.open(fileName.c_str(), std::ios::out | std::ios::binary);
		oFile.seekp(0);
		oFile.write((char*)&vSize, sizeof(unsigned int));
		oFile.write((char*)&iSize, sizeof(unsigned int));

		for (auto v : _vertices)
			oFile.write((char*)&v, sizeof(vertex));

		for (auto i : _indices)
			oFile.write((char*)&i, sizeof(unsigned int));

		oFile.close();
	}

	mesh* mesh::fromMb(std::string fileName)
	{
		LOG("FromMb: Loading " << fileName);

		std::string line = std::string();

		if (!globals::contains(fileName, ".mb"))
		{
			LOG("Read OBJ: Invalid file format.");
			return nullptr;
		}

		std::vector<vertex> vertices; 
		std::vector<GLuint> indices;

		GLuint vSize;
		GLuint iSize;

		std::ifstream iFile;
		iFile.open(fileName.c_str(), std::ios::in | std::ios::binary);
		iFile.seekg(0);
		iFile.read ((char*)&vSize, sizeof(GLuint));
		iFile.read ((char*)&iSize, sizeof(GLuint));

		GLuint vCount = vSize / (sizeof(vertex));
		GLuint iCount = iSize / (sizeof(GLuint));

		for(GLuint i = 0; i < vCount; i++)
		{
			vertex v;
			iFile.read((char*)&v, sizeof (vertex));    
			vertices.push_back(v);
		}

		for(GLuint j = 0; j < iCount; j++)
		{
			GLuint i;
			iFile.read ((char*)&i, sizeof (GLuint));
			indices.push_back(i);
		}

		iFile.close();

		mesh* _mesh = new mesh();
		_mesh->addVertices(vertices, indices);

		return _mesh;
	}

	mesh* mesh::fromObj(const std::string fileName, bool shouldCalcNormals)
	{   
		LOG("FromObj: Loading " << fileName);

		std::string line = std::string();

		if (!globals::contains(fileName, ".model"))
		{
			LOG("Read OBJ: Invalid file format.");
			return nullptr;
		}

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec3> normals;

		std::vector<GLuint> pIndexes;
		std::vector<GLuint> tIndexes;
		std::vector<GLuint> nIndexes;

		std::vector<GLuint> indices;

		//read obj and fill vectors of indices
		std::ifstream file(fileName);

		if (file.is_open()) 
		{
			while (!file.eof()) 
			{
				getline(file, line);

				std::string data;

				std::string token = getToken(line);

				if (token == "v")
				{
					data = line.substr(2);
					std::vector<std::string> values = globals::split(data, ' ');

					float x = (float)atof(values[0].c_str());
					float y = (float)atof(values[1].c_str());
					float z = (float)atof(values[2].c_str());

					positions.push_back(glm::vec3(x, y, z));
				}
				else if (token == "vt")
				{
					data = line.substr(3);
					std::vector<std::string> values = globals::split(data, ' ');

					float u = (float)atof(values[0].c_str());
					float v = (float)atof(values[1].c_str());

					texCoords.push_back(glm::vec2(u, v));
				}
				else if (token == "vn")
				{
					data = line.substr(3);
					std::vector<std::string> values = globals::split(data, ' ');

					float r = (float)atof(values[0].c_str());
					float s = (float)atof(values[1].c_str());
					float t = (float)atof(values[2].c_str());

					normals.push_back(glm::vec3(r, s, t));
				}
				else if (token == "f")
				{
					data = line.substr(2);
					std::vector<std::string> faceString = globals::split(data, ' ');

					for(GLuint i = 0; i < faceString.size(); i++)
					{
						std::vector<std::string> vertexString = globals::split(faceString[i], '/');

						if (vertexString.size() >= 1)
							pIndexes.push_back(atoi(vertexString[0].c_str()) - 1);

						if (vertexString.size() >= 2 && !vertexString[1].empty())
							tIndexes.push_back(atoi( vertexString[1].c_str()) - 1);

						if (vertexString.size() == 3 && !vertexString[2].empty())
							nIndexes.push_back(atoi(vertexString[2].c_str()) - 1);
					}
				}
			}

			file.close();
		}

		//create vertices
		GLuint pSize = pIndexes.size();
		GLuint tSize = tIndexes.size();
		GLuint nSize = nIndexes.size();

		std::vector<vertex> tempVertices;

		for(GLuint i = 0; i < pSize; i++)
		{
			int pIndex = pIndexes[i];

			glm::vec3 position = positions[pIndex];

			glm::vec3 normal = glm::vec3(0.0f);
			glm::vec2 texCoord = glm::vec2(0.0f);

			if (tSize > 0)
				texCoord = texCoords[tIndexes[i]];

			if (nSize > 0 && !shouldCalcNormals)
				normal = normals[nIndexes[i]];

			tempVertices.push_back(vertex(position, texCoord, normal));
		}

		//select vertices and create indices
		std::vector<vertex> vertices;
		GLuint vSize = tempVertices.size();

		vertex vertex;
		bool exists = false;
		int index = 0;
		GLuint i = 0;
		GLuint j = 0;
		GLuint verticesSize = 0;

		for(i = 0; i < vSize; i++)
		{
			vertex = tempVertices[i];
			exists = false;
			index = 0;

			for(j = 0; j < verticesSize; j++)
			{
				if (vertices[j] == vertex)
				{
					exists = true;
					break;
				}

				index++;
			}

			if (!exists)
			{
				index = verticesSize++;
				vertices.push_back(vertex);
			}

			indices.push_back(index);
		}

		//calc normals
		if (shouldCalcNormals || nSize == 0)
			calcNormals(vertices, indices);

		//calc tangents
		if (tSize > 0)
			calcTangents(vertices, indices);

		return mesh::create(vertices, indices);
	}

	void mesh::calcNormals(std::vector<vertex> &vertices, std::vector<GLuint> indices)
	{
		for(GLuint i = 0; i< indices.size(); i += 3)
		{
			GLuint i0 = indices[i + 0];
			GLuint i1 = indices[i + 1];
			GLuint i2 = indices[i + 2];

			glm::vec3 a = vertices[i0].getPosition();
			glm::vec3 b = vertices[i1].getPosition();
			glm::vec3 c = vertices[i2].getPosition();

			glm::vec3 v0 = b - a;
			glm::vec3 v1 = c - a;

			glm::vec3 normal = glm::cross(v0, v1);
			normal = glm::normalize(normal);

			glm::vec3 n0 = vertices[i0].getNormal(); 
			glm::vec3 n1 = vertices[i1].getNormal();
			glm::vec3 n2 = vertices[i2].getNormal();

			vertices[i0].setNormal(n0 + normal);
			vertices[i1].setNormal(n1 + normal);
			vertices[i2].setNormal(n2 + normal);
		}

		for(GLuint i = 0; i < vertices.size(); i++)
		{
			glm::vec3 normal = vertices[i].getNormal();
			normal = glm::normalize(normal);
			vertices[i].setNormal(normal);
		}
	}

	void mesh::calcTangents(std::vector<vertex> &vertices, std::vector<GLuint> indices)
	{
		for(GLuint i = 0; i< indices.size(); i += 3)
		{
			GLuint i0 = indices[i + 0];
			GLuint i1 = indices[i + 1];
			GLuint i2 = indices[i + 2];

			glm::vec3 p0 = vertices[i0].getPosition();
			glm::vec3 p1 = vertices[i1].getPosition();
			glm::vec3 p2 = vertices[i2].getPosition();

			glm::vec3 v0 = p1 - p0;
			glm::vec3 v1 = p2 - p0;

			glm::vec2 tc0 = vertices[i0].getTexCoord();
			glm::vec2 tc1 = vertices[i1].getTexCoord();
			glm::vec2 tc2 = vertices[i2].getTexCoord();

			glm::vec2 uv0 = tc1-tc0;
			glm::vec2 uv1 = tc2-tc0;

			float r = 1.0f / (uv0.x * uv1.y - uv0.y * uv1.x);
			glm::vec3 tangent = glm::normalize((v0 * uv1.y - v1 * uv0.y) * r);
			glm::vec3 bitangent = glm::normalize((v1 * uv0.x - v0 * uv1.x) * r);

			vertices[i0].setTangent(tangent);
			vertices[i1].setTangent(tangent);
			vertices[i2].setTangent(tangent);

			vertices[i0].setBitangent(bitangent);
			vertices[i1].setBitangent(bitangent);
			vertices[i2].setBitangent(bitangent);
		}

		for (GLuint i = 0; i < vertices.size(); i++)
		{
			glm::vec3 n = vertices[i].getNormal();
			glm::vec3 t = vertices[i].getTangent();
			glm::vec3 b = vertices[i].getBitangent();

			// Gram-Schmidt orthogonalize
			t = glm::normalize(t - n * glm::dot(n, t));

			if (glm::dot(glm::cross(n, t), b) < 0.0f)
				t = t * -1.0f;

			vertices[i].setTangent(t);
		}
	}
}
