#include <phi/core/geometryData.h>
#include <phi/io/path.h>

#include <phi/core/globals.h>

#include <phi/io/path.h>

#include <fstream>
#include <iostream>
#include <vector>

namespace phi
{
    geometryData::geometryData()
    {
    }

    geometryData::~geometryData()
    {
        delete[] _positionsBuffer;
        delete[] _texCoordsBuffer;
        delete[] _normalsBuffer;
        delete[] _tangentsBuffer;
    }

    geometryData* geometryData::create(std::vector<vertex> &vertices, std::vector<uint>* indices)
    {
        geometryData* m = new geometryData();
        m->addVertices(vertices, indices);
        return m;
    }

    geometryData* geometryData::create(
        uint verticesCount,
        float* positionsBuffer,
        float* texCoordsBuffer,
        float* normalsBuffer,
        uint indicesCount,
        uint* indicesBuffer)
    {
        auto indices = new std::vector<uint>();

        for (uint i = 0; i < indicesCount; i++)
            indices->push_back(indicesBuffer[i]);

        std::vector<vertex> vertices;

        for (uint i = 0; i < verticesCount; i++)
        {
            float x = positionsBuffer[i * 3 + 0];
            float y = positionsBuffer[i * 3 + 1];
            float z = positionsBuffer[i * 3 + 2];

            vec3 position = vec3(x, y, z);

            float u = texCoordsBuffer[i * 2 + 0];
            float v = texCoordsBuffer[i * 2 + 1];

            vec2 texCoord = vec2(u, v);

            float r = normalsBuffer[i * 3 + 0];
            float s = normalsBuffer[i * 3 + 1];
            float t = normalsBuffer[i * 3 + 2];

            vec3 normal = vec3(r, s, t);

            vertex vert = vertex(position, texCoord, normal);

            vertices.push_back(vert);
        }

        geometryData::calcTangents(vertices, *indices);

        unsigned int tgIndex = 0;
        float* tangentsBuffer = new float[verticesCount * 3];

        for (uint i = 0; i < verticesCount; i++)
        {
            vertex vertex = vertices[i];

            float r1 = vertex.getTangent().x;
            float s1 = vertex.getTangent().y;
            float t1 = vertex.getTangent().z;

            tangentsBuffer[tgIndex++] = r1;
            tangentsBuffer[tgIndex++] = s1;
            tangentsBuffer[tgIndex++] = t1;
        }

        geometryData* g = new geometryData();
        auto floatSize = (uint)sizeof(float);

        g->_indices = indices;
        g->_vertices = vertices;
        g->_indicesCount = indicesCount;
        g->_positionsBuffer = positionsBuffer;
        g->_texCoordsBuffer = texCoordsBuffer;
        g->_normalsBuffer = normalsBuffer;
        g->_tangentsBuffer = tangentsBuffer;
        g->_pSize = verticesCount * 3u * floatSize;
        g->_tSize = verticesCount * 2u * floatSize;
        g->_nSize = verticesCount * 3u * floatSize;
        g->_tgSize = verticesCount * 3u * floatSize;

        return g;
    }

    void geometryData::addVertices(std::vector<vertex> vertices, std::vector<uint>* indices)
    {
        _vertices = vertices;
        _indices = indices;
        _indicesCount = (uint)indices->size();

        auto verticesCount = (uint)vertices.size();
        auto sizeOfFloat = (uint)sizeof(float);

        _pSize = verticesCount * 3 * sizeOfFloat;
        _tSize = verticesCount * 2 * sizeOfFloat;
        _nSize = verticesCount * 3 * sizeOfFloat;
        _tgSize = verticesCount * 3 * sizeOfFloat;

        _positionsBuffer = new float[verticesCount * 3];
        _texCoordsBuffer = new float[verticesCount * 2];
        _normalsBuffer = new float[verticesCount * 3];
        _tangentsBuffer = new float[verticesCount * 3];

        createBuffers(vertices, _positionsBuffer, _texCoordsBuffer, _normalsBuffer, _tangentsBuffer);
    }

    void geometryData::createBuffers(
        std::vector<vertex> vertices, 
        float* &positionsBuffer, 
        float* &texCoordsBuffer, 
        float* &normalsBuffer, 
        float* &tangentsBuffer)
    {
        std::vector<vertex>::iterator i;
        unsigned int pIndex = 0;
        unsigned int tIndex = 0;
        unsigned int nIndex = 0;
        unsigned int tgIndex = 0;

        for (i = vertices.begin(); i != vertices.end(); ++i)
        {
            vertex vertex = *i;

            float x = vertex.getPosition().x;
            float y = vertex.getPosition().y;
            float z = vertex.getPosition().z;

            float u = vertex.getTexCoord().x;
            float v = vertex.getTexCoord().y;

            float r0 = vertex.getNormal().x;
            float s0 = vertex.getNormal().y;
            float t0 = vertex.getNormal().z;

            float r1 = vertex.getTangent().x;
            float s1 = vertex.getTangent().y;
            float t1 = vertex.getTangent().z;

            positionsBuffer[pIndex++] = x;
            positionsBuffer[pIndex++] = y;
            positionsBuffer[pIndex++] = z;

            texCoordsBuffer[tIndex++] = u;
            texCoordsBuffer[tIndex++] = v;

            normalsBuffer[nIndex++] = r0;
            normalsBuffer[nIndex++] = s0;
            normalsBuffer[nIndex++] = t0;

            tangentsBuffer[tgIndex++] = r1;
            tangentsBuffer[tgIndex++] = s1;
            tangentsBuffer[tgIndex++] = t1;
        }
    }

    void geometryData::calcNormals(std::vector<vertex> &vertices, std::vector<uint> &indices)
    {
        for (uint i = 0; i < indices.size(); i += 3)
        {
            uint i0 = indices[i + 0];
            uint i1 = indices[i + 1];
            uint i2 = indices[i + 2];

            vec3 a = vertices[i0].getPosition();
            vec3 b = vertices[i1].getPosition();
            vec3 c = vertices[i2].getPosition();

            vec3 v0 = b - a;
            vec3 v1 = c - a;

            vec3 normal = cross(v0, v1);
            normal = normalize(normal);

            vec3 n0 = vertices[i0].getNormal();
            vec3 n1 = vertices[i1].getNormal();
            vec3 n2 = vertices[i2].getNormal();

            vertices[i0].setNormal(n0 + normal);
            vertices[i1].setNormal(n1 + normal);
            vertices[i2].setNormal(n2 + normal);
        }

        for (uint i = 0; i < vertices.size(); i++)
        {
            vec3 normal = vertices[i].getNormal();
            normal = normalize(normal);
            vertices[i].setNormal(normal);
        }
    }

    void geometryData::calcTangents(std::vector<vertex> &vertices, std::vector<uint> &indices)
    {
        for (uint i = 0; i < indices.size(); i += 3)
        {
            uint i0 = indices[i + 0];
            uint i1 = indices[i + 1];
            uint i2 = indices[i + 2];

            vec3 p0 = vertices[i0].getPosition();
            vec3 p1 = vertices[i1].getPosition();
            vec3 p2 = vertices[i2].getPosition();

            vec3 v0 = p1 - p0;
            vec3 v1 = p2 - p0;

            vec2 tc0 = vertices[i0].getTexCoord();
            vec2 tc1 = vertices[i1].getTexCoord();
            vec2 tc2 = vertices[i2].getTexCoord();

            vec2 uv0 = tc1 - tc0;
            vec2 uv1 = tc2 - tc0;

            float r = 1.0f / (uv0.x * uv1.y - uv0.y * uv1.x);
            vec3 tangent = normalize((v0 * uv1.y - v1 * uv0.y) * r);
            vec3 bitangent = normalize((v1 * uv0.x - v0 * uv1.x) * r);

            vertices[i0].setTangent(tangent);
            vertices[i1].setTangent(tangent);
            vertices[i2].setTangent(tangent);

            vertices[i0].setBitangent(bitangent);
            vertices[i1].setBitangent(bitangent);
            vertices[i2].setBitangent(bitangent);
        }

        for (uint i = 0; i < vertices.size(); i++)
        {
            vec3 n = vertices[i].getNormal();
            vec3 t = vertices[i].getTangent();
            vec3 b = vertices[i].getBitangent();

            // Gram-Schmidt orthogonalize
            t = normalize(t - n * dot(n, t));

            if (dot(cross(n, t), b) < 0.0f)
                t = t * -1.0f;

            vertices[i].setTangent(t);
        }
    }
}
