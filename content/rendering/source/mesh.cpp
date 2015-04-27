#include "mesh.h"
#include "globals.h"
#include "path.h"

#include <fstream>
#include <iostream>
#include <vector>

namespace phi
{
	mesh::mesh()
	{
		_vao = 0;

		_positionsVbo = 0;
		_texCoordsVbo = 0;
		_normalsVbo = 0;
		_tangentsVbo = 0;
		_indicesVbo = 0;
		_indicesCount = 0;

		_isBound = false;
        _isSelected = false;
	}

	mesh::~mesh()

	{   delete[] _positionsBuffer;
		delete[] _texCoordsBuffer;
		delete[] _normalsBuffer;
		delete[] _tangentsBuffer;

		glDeleteBuffers(1, &_positionsVbo);
		glDeleteBuffers(1, &_texCoordsVbo);
		glDeleteBuffers(1, &_normalsVbo);
		glDeleteBuffers(1, &_tangentsVbo);
		glDeleteBuffers(1, &_indicesVbo);
		glDeleteVertexArrays(1, &_vao);
	}

	mesh* mesh::create(std::string name, std::vector<vertex> &vertices, std::vector<GLuint> &indices)
	{
		mesh* m = new mesh();
		m->addVertices(vertices, indices);
		return m;
	}

    mesh* mesh::create(
            GLuint verticesCount, 
            GLfloat* positionsBuffer, 
            GLfloat* texCoordsBuffer, 
            GLfloat* normalsBuffer, 
            GLuint indicesCount, 
            GLuint* indicesBuffer, 
            std::string materialName)
    {
        std::vector<GLuint> indices;

        for (GLuint i = 0; i < indicesCount; i++)
            indices.push_back(indicesBuffer[i]);

        std::vector<vertex> vertices;

        for (GLuint i = 0; i < verticesCount; i++)
        {
            float x = positionsBuffer[i * 3 + 0];
            float y = positionsBuffer[i * 3 + 1];
            float z = positionsBuffer[i * 3 + 2];

            glm::vec3 position = glm::vec3(x, y, z);

            float u = texCoordsBuffer[i * 2 + 0];
            float v = texCoordsBuffer[i * 2 + 1];
            
            glm::vec2 texCoord = glm::vec2(u, v);

            float r = normalsBuffer[i * 3 + 0];
            float s = normalsBuffer[i * 3 + 1];
            float t = normalsBuffer[i * 3 + 2];

            glm::vec3 normal = glm::vec3(r, s, t);

            vertex vert = vertex(position, texCoord, normal);

            vertices.push_back(vert);
        }

        mesh::calcTangents(vertices, indices);

        unsigned int tgIndex = 0;
		GLfloat* tangentsBuffer = new GLfloat[verticesCount * 3];

		for (GLuint i = 0; i < verticesCount; i++)
		{
			vertex vertex = vertices[i];

			GLfloat r1 = vertex.getTangent().x;
			GLfloat s1 = vertex.getTangent().y;
			GLfloat t1 = vertex.getTangent().z;

			tangentsBuffer[tgIndex++] = r1;
			tangentsBuffer[tgIndex++] = s1;
			tangentsBuffer[tgIndex++] = t1;
		}

        mesh* m = new mesh();
        auto floatSize = sizeof(GLfloat);
        m->_indices = indices;
        m->_vertices = vertices;
        m->_indicesCount = indicesCount;
        m->_positionsBuffer = positionsBuffer;
        m->_texCoordsBuffer = texCoordsBuffer;
        m->_normalsBuffer = normalsBuffer;
        m->_tangentsBuffer = tangentsBuffer;
        m->_pSize = verticesCount * 3 * floatSize;
        m->_tSize = verticesCount * 2 * floatSize;
        m->_nSize = verticesCount * 3 * floatSize;
        m->_tgSize = verticesCount * 3 * floatSize;
        m->_materialName = materialName;

        m->storeBuffers();

        return m;
    }

	void mesh::addVertices(std::vector<vertex> vertices, std::vector<GLuint> indices)
	{
		_vertices = vertices;
		_indices = indices;
		_indicesCount = indices.size();

		_pSize = vertices.size() * 3 * sizeof(GLfloat);
		_tSize = vertices.size() * 2 * sizeof(GLfloat);
		_nSize = vertices.size() * 3 * sizeof(GLfloat);
		_tgSize = vertices.size() * 3 * sizeof(GLfloat);

		_positionsBuffer = new GLfloat[vertices.size() * 3];
		_texCoordsBuffer = new GLfloat[vertices.size() * 2];
		_normalsBuffer = new GLfloat[vertices.size() * 3];
		_tangentsBuffer = new GLfloat[vertices.size() * 3];

		createBuffers(vertices, _positionsBuffer, _texCoordsBuffer, _normalsBuffer, _tangentsBuffer);

		storeBuffers();
	}

    void mesh::storeBuffers()
    {
        auto indicesSize = _indicesCount * sizeof(GLuint);

        glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		glGenBuffers(1, &_positionsVbo);
		glBindBuffer(GL_ARRAY_BUFFER, _positionsVbo);
		glBufferData(GL_ARRAY_BUFFER, _pSize, _positionsBuffer, GL_STATIC_DRAW);

		glGenBuffers(1, &_texCoordsVbo);
		glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
		glBufferData(GL_ARRAY_BUFFER, _tSize, _texCoordsBuffer, GL_STATIC_DRAW);

		glGenBuffers(1, &_normalsVbo);
		glBindBuffer(GL_ARRAY_BUFFER, _normalsVbo);
		glBufferData(GL_ARRAY_BUFFER, _nSize, _normalsBuffer, GL_STATIC_DRAW);

		glGenBuffers(1, &_tangentsVbo);
		glBindBuffer(GL_ARRAY_BUFFER, _tangentsVbo);
		glBufferData(GL_ARRAY_BUFFER, _tgSize, _tangentsBuffer, GL_STATIC_DRAW);

		glGenBuffers(1, &_indicesVbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, &_indices[0], GL_STATIC_DRAW);
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
		glBindBuffer(GL_ARRAY_BUFFER, _positionsVbo);
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

    void mesh::setMaterial(material* value)
    { 
        _material = value; 

        if (_material != nullptr)
            _materialName = value->getName(); 
    }

	void mesh::saveToModel(std::string fileName)
	{
		LOG("saveToModel: " << fileName);

		std::ofstream oFile;
	    oFile.open(fileName.c_str(), std::ios::out | std::ios::binary);

        if (!oFile.is_open())
            return;

	    oFile.seekp(0);
        oFile.write((char*)&_pSize, sizeof(GLuint));
        oFile.write(reinterpret_cast<const char*>(_positionsBuffer), _pSize);

        oFile.write((char*)&_tSize, sizeof(GLuint));
        oFile.write(reinterpret_cast<const char*>(_texCoordsBuffer), std::streamsize(_tSize));

        oFile.write((char*)&_nSize, sizeof(GLuint));
        oFile.write(reinterpret_cast<const char*>(_normalsBuffer), std::streamsize(_nSize));

        oFile.write((char*)&_iSize, sizeof(GLuint));
        oFile.write(reinterpret_cast<const char*>(_indicesBuffer), std::streamsize(_iSize));

        oFile.write((char*)_material->getName().c_str(), 256);

        oFile.close();
	}

	mesh* mesh::fromMesh(std::string fileName)
	{
		LOG("fromMesh: " << fileName);

		if (!globals::contains(fileName, ".mesh"))
		{
			LOG("fromMesh: Invalid file format.");
			return nullptr;
		}

		mesh* m = new mesh();

        GLuint pSize = -1;
        char* materialName = new char[256];

	    std::ifstream iFile;
	    iFile.open(fileName.c_str(), std::ios::in | std::ios::binary);

        if (!iFile.is_open())
        {
            LOG("fromMesh: Could not load the file.");
            return nullptr;
        }

	    iFile.seekg(0);
    
        iFile.read ((char*)&m->_pSize, sizeof(GLuint));
        m->_positionsBuffer = new GLfloat[m->_pSize / sizeof(GLfloat)];
        iFile.read(reinterpret_cast<char*>(m->_positionsBuffer), std::streamsize(m->_pSize));

        iFile.read ((char*)&m->_tSize, sizeof(GLuint));
        m->_texCoordsBuffer = new GLfloat[m->_tSize / sizeof(GLfloat)];
        iFile.read(reinterpret_cast<char*>(m->_texCoordsBuffer), std::streamsize(m->_tSize));

        iFile.read ((char*)&m->_nSize, sizeof(GLuint));
        m->_normalsBuffer = new GLfloat[m->_nSize / sizeof(GLfloat)];
        iFile.read(reinterpret_cast<char*>(m->_normalsBuffer), std::streamsize(m->_nSize));

        iFile.read ((char*)&m->_iSize, sizeof(GLuint));
        m->_indicesBuffer = new GLuint[m->_iSize / sizeof(GLuint)];
        iFile.read(reinterpret_cast<char*>(m->_indicesBuffer), std::streamsize(m->_iSize));

        iFile.read(materialName, 256);
        m->_materialName = materialName;
        
        m->_indicesCount = m->_iSize / sizeof(GLuint);

        for (GLuint i = 0; i < m->_indicesCount; i++)
            m->_indices.push_back(m->_indicesBuffer[i]);

        GLuint verticesCount = m->_pSize / sizeof(GLfloat) / 3;

        for (GLuint i = 0; i < verticesCount; i++)
        {
            float x = m->_positionsBuffer[i * 3 + 0];
            float y = m->_positionsBuffer[i * 3 + 1];
            float z = m->_positionsBuffer[i * 3 + 2];

            glm::vec3 position = glm::vec3(x, y, z);

            float u = m->_texCoordsBuffer[i * 2 + 0];
            float v = m->_texCoordsBuffer[i * 2 + 1];
            
            glm::vec2 texCoord = glm::vec2(u, v);

            float r = m->_normalsBuffer[i * 3 + 0];
            float s = m->_normalsBuffer[i * 3 + 1];
            float t = m->_normalsBuffer[i * 3 + 2];

            glm::vec3 normal = glm::vec3(r, s, t);

            vertex vert = vertex(position, texCoord, normal);

            m->_vertices.push_back(vert);
        }

        m->calcTangents(m->_vertices, m->_indices);

		unsigned int tgIndex = 0;
		m->_tangentsBuffer = new GLfloat[verticesCount * 3];

		for (GLuint i = 0; i < verticesCount; i++)
		{
			vertex vertex = m->_vertices[i];

			GLfloat r1 = vertex.getTangent().x;
			GLfloat s1 = vertex.getTangent().y;
			GLfloat t1 = vertex.getTangent().z;

			m->_tangentsBuffer[tgIndex++] = r1;
			m->_tangentsBuffer[tgIndex++] = s1;
			m->_tangentsBuffer[tgIndex++] = t1;
		}

        m->_tgSize = m->_nSize;

        glGenVertexArrays(1, &m->_vao);
        glBindVertexArray(m->_vao);

        glGenBuffers(1, &m->_positionsVbo);
        glBindBuffer(GL_ARRAY_BUFFER, m->_positionsVbo);
        glBufferData(GL_ARRAY_BUFFER, m->_pSize, m->_positionsBuffer, GL_STATIC_DRAW);

        glGenBuffers(1, &m->_texCoordsVbo);
        glBindBuffer(GL_ARRAY_BUFFER, m->_texCoordsVbo);
        glBufferData(GL_ARRAY_BUFFER, m->_tSize, m->_texCoordsBuffer, GL_STATIC_DRAW);

        glGenBuffers(1, &m->_normalsVbo);
        glBindBuffer(GL_ARRAY_BUFFER, m->_normalsVbo);
        glBufferData(GL_ARRAY_BUFFER, m->_nSize, m->_normalsBuffer, GL_STATIC_DRAW);

        glGenBuffers(1, &m->_tangentsVbo);
		glBindBuffer(GL_ARRAY_BUFFER, m->_tangentsVbo);
		glBufferData(GL_ARRAY_BUFFER, m->_tgSize, m->_tangentsBuffer, GL_STATIC_DRAW);

        glGenBuffers(1, &m->_indicesVbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->_indicesVbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->_iSize, m->_indicesBuffer, GL_STATIC_DRAW);

	    return m;
	}

	mesh* mesh::fromObj(std::string name, const std::string fileName, bool shouldCalcNormals)
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

		return mesh::create(name, vertices, indices);
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
