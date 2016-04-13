#include <precompiled.h>
#include "geometry.h"
#include <io\path.h>

#include <iterator>

namespace phi
{
    geometry* geometry::_quad = nullptr;

    geometry::~geometry()
    {
        safeDeleteArray(vboData);
        safeDeleteArray(eboData);
        safeDelete(aabb);
    }

    geometry* geometry::create(vector<vertex> vertices, vector<uint> indices)
    {
        calcTangents(vertices, indices);

        auto data = new geometry();
        auto verticesCount = vertices.size();
        auto indicesCount = indices.size();

        data->verticesCount = (uint)verticesCount;
        data->indicesCount = (uint)indicesCount;
        data->vboSize = (uint)(verticesCount * sizeof(vertex));
        data->eboSize = (uint)(data->indicesCount * sizeof(uint));

        data->vboData = new vertex[verticesCount];
        data->eboData = new uint[indicesCount];

        memcpy(data->vboData, &vertices[0], data->vboSize);
        memcpy(data->eboData, &indices[0], data->eboSize);

        data->aabb = new phi::aabb(calcAabb(vertices));

        return data;
    }

    geometry* geometry::create(
        uint verticesCount,
        float* positionsBuffer,
        float* texCoordsBuffer,
        float* normalsBuffer,
        uint indicesCount,
        uint* indicesBuffer)
    {
        vector<uint> indices(indicesBuffer, indicesBuffer + indicesCount);
        vector<vertex> vertices;

        for (uint i = 0; i < verticesCount; i++)
        {
            auto x = positionsBuffer[i * 3 + 0];
            auto y = positionsBuffer[i * 3 + 1];
            auto z = positionsBuffer[i * 3 + 2];

            auto position = vec3(x, y, z);

            auto u = texCoordsBuffer[i * 2 + 0];
            auto v = texCoordsBuffer[i * 2 + 1];

            auto texCoord = vec2(u, v);

            auto r = normalsBuffer[i * 3 + 0];
            auto s = normalsBuffer[i * 3 + 1];
            auto t = normalsBuffer[i * 3 + 2];

            auto normal = vec3(r, s, t);

            vertices.push_back(vertex(position, texCoord, normal));
        }

        return geometry::create(vertices, indices);
    }

    void geometry::calcNormals(vector<vertex>& vertices, vector<uint>& indices)
    {
        for (uint i = 0; i < indices.size(); i += 3)
        {
            auto i0 = indices[i + 0];
            auto i1 = indices[i + 1];
            auto i2 = indices[i + 2];

            auto a = vertices[i0].position;
            auto b = vertices[i1].position;
            auto c = vertices[i2].position;

            auto v0 = b - a;
            auto v1 = c - a;

            auto normal = cross(v0, v1);
            normal = normalize(normal);

            auto n0 = vertices[i0].normal;
            auto n1 = vertices[i1].normal;
            auto n2 = vertices[i2].normal;

            vertices[i0].normal = n0 + normal;
            vertices[i1].normal = n1 + normal;
            vertices[i2].normal = n2 + normal;
        }

        for (uint i = 0; i < vertices.size(); i++)
        {
            auto normal = vertices[i].normal;
            normal = normalize(normal);
            vertices[i].normal = normal;
        }
    }

    void geometry::calcTangents(vector<vertex>& vertices, vector<uint>& indices)
    {
        auto verticesCount = vertices.size();
        auto indicesCount = indices.size();

        auto bitangents = new vec3[verticesCount];

        for (uint i = 0; i < indicesCount; i += 3)
        {
            auto i0 = indices[i + 0];
            auto i1 = indices[i + 1];
            auto i2 = indices[i + 2];

            auto p0 = vertices[i0].position;
            auto p1 = vertices[i1].position;
            auto p2 = vertices[i2].position;

            auto v0 = p1 - p0;
            auto v1 = p2 - p0;

            auto tc0 = vertices[i0].texCoord;
            auto tc1 = vertices[i1].texCoord;
            auto tc2 = vertices[i2].texCoord;

            auto uv0 = tc1 - tc0;
            auto uv1 = tc2 - tc0;

            float r = 1.0f / (uv0.x * uv1.y - uv0.y * uv1.x);
            auto tangent = glm::normalize((v0 * uv1.y - v1 * uv0.y) * r);
            auto bitangent = glm::normalize((v1 * uv0.x - v0 * uv1.x) * r);

            vertices[i0].tangent = tangent;
            vertices[i1].tangent = tangent;
            vertices[i2].tangent = tangent;

            bitangents[i0] = bitangent;
            bitangents[i1] = bitangent;
            bitangents[i2] = bitangent;
        }

        for (uint i = 0; i < verticesCount; i++)
        {
            auto n = vertices[i].normal;
            auto t = vertices[i].tangent;
            auto b = bitangents[i];

            // Gram-Schmidt orthogonalize
            t = glm::normalize(t - n * dot(n, t));

            if (dot(cross(n, t), b) < 0.0f)
                t = t * -1.0f;

            vertices[i].tangent = t;
        }

        safeDeleteArray(bitangents);
    }

    aabb geometry::calcAabb(vector<vertex>& vertices)
    {
        auto verticesCount = vertices.size();
        if (verticesCount == 0)
            return phi::aabb();

        auto firstVertex = vertices[0].position;
        auto minX = firstVertex.x;
        auto minY = firstVertex.y;
        auto minZ = firstVertex.z;
        auto maxX = firstVertex.x;
        auto maxY = firstVertex.y;
        auto maxZ = firstVertex.z;

        for (size_t i = 1; i < verticesCount; i++)
        {
            auto p = vertices[i].position;
            minX = glm::min(minX, p.x);
            minY = glm::min(minY, p.y);
            minZ = glm::min(minZ, p.z);
            maxX = glm::max(maxX, p.x);
            maxY = glm::max(maxY, p.y);
            maxZ = glm::max(maxZ, p.z);
        }

        return phi::aabb(vec3(minX, minY, minZ), vec3(maxX, maxY, maxZ));
    }

    geometry* geometry::quad()
    {
        if (_quad == nullptr)
        {
            vector<vertex> vertices
            {
                vertex(vec3(-0.5f, -0.5f, 0.0f), vec2(0.0f, 0.0f)),
                vertex(vec3(+0.5f, -0.5f, 0.0f), vec2(1.0f, 0.0f)),
                vertex(vec3(+0.5f, +0.5f, 0.0f), vec2(1.0f, 1.0f)),
                vertex(vec3(-0.5f, +0.5f, 0.0f), vec2(0.0f, 1.0f)),
            };

            vector<uint> indices { 0, 1, 2, 2, 3, 0 };

            _quad = create(vertices, indices);
        }

        return _quad;
    }
}
