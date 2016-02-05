#include <phi\core\geometry.h>
#include <phi\io\path.h>

namespace phi
{
    geometry* geometry::_quad = nullptr;

    geometry* geometry::create(std::vector<vertex> vertices, std::vector<uint> indices)
    {
        calcTangents(vertices, indices);

        auto data = new geometry();
        auto verticesCount = vertices.size();
        auto indicesCount = indices.size();

        data->verticesCount = verticesCount;
        data->indicesCount = indicesCount;
        data->vboSize = verticesCount * sizeof(vertex);
        data->eboSize = data->indicesCount * sizeof(uint);
        memcpy(data->vboData, &vertices[0], data->vboSize);
        memcpy(data->eboData, &indices[0], data->eboSize);

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
        std::vector<uint> indices(indicesCount, *indicesBuffer);

        std::vector<vertex> vertices;

        for (auto i = 0; i < verticesCount; i++)
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

        calcTangents(vertices, indices);

        unsigned int tgIndex = 0;
        auto tangentsBuffer = new float[verticesCount * 3];

        for (auto i = 0; i < verticesCount; i++)
        {
            auto vertex = vertices[i];

            auto r1 = vertex.tangent.x;
            auto s1 = vertex.tangent.y;
            auto t1 = vertex.tangent.z;

            tangentsBuffer[tgIndex++] = r1;
            tangentsBuffer[tgIndex++] = s1;
            tangentsBuffer[tgIndex++] = t1;
        }

        return geometry::create(vertices, indices);
    }

    void geometry::calcNormals(std::vector<vertex> &vertices, std::vector<uint> &indices)
    {
        for (uint i = 0; i < indices.size(); i += 3)
        {
            uint i0 = indices[i + 0];
            uint i1 = indices[i + 1];
            uint i2 = indices[i + 2];

            vec3 a = vertices[i0].position;
            vec3 b = vertices[i1].position;
            vec3 c = vertices[i2].position;

            vec3 v0 = b - a;
            vec3 v1 = c - a;

            vec3 normal = cross(v0, v1);
            normal = normalize(normal);

            vec3 n0 = vertices[i0].normal;
            vec3 n1 = vertices[i1].normal;
            vec3 n2 = vertices[i2].normal;

            vertices[i0].normal = n0 + normal;
            vertices[i1].normal = n1 + normal;
            vertices[i2].normal = n2 + normal;
        }

        for (uint i = 0; i < vertices.size(); i++)
        {
            vec3 normal = vertices[i].normal;
            normal = normalize(normal);
            vertices[i].normal = normal;
        }
    }

    void geometry::calcTangents(std::vector<vertex> &vertices, std::vector<uint> &indices)
    {
        std::vector<vec3> bitangents;
        bitangents.reserve(vertices.size());

        auto indicesCount = indices.size();

        for (uint i = 0; i < indicesCount; i += 3)
        {
            uint i0 = indices[i + 0];
            uint i1 = indices[i + 1];
            uint i2 = indices[i + 2];

            vec3 p0 = vertices[i0].position;
            vec3 p1 = vertices[i1].position;
            vec3 p2 = vertices[i2].position;

            vec3 v0 = p1 - p0;
            vec3 v1 = p2 - p0;

            vec2 tc0 = vertices[i0].texCoord;
            vec2 tc1 = vertices[i1].texCoord;
            vec2 tc2 = vertices[i2].texCoord;

            vec2 uv0 = tc1 - tc0;
            vec2 uv1 = tc2 - tc0;

            float r = 1.0f / (uv0.x * uv1.y - uv0.y * uv1.x);
            vec3 tangent = normalize((v0 * uv1.y - v1 * uv0.y) * r);
            vec3 bitangent = normalize((v1 * uv0.x - v0 * uv1.x) * r);

            vertices[i0].tangent = tangent;
            vertices[i1].tangent = tangent;
            vertices[i2].tangent = tangent;

            bitangents[i0] = bitangent;
            bitangents[i1] = bitangent;
            bitangents[i2] = bitangent;
        }

        for (uint i = 0; i < vertices.size(); i++)
        {
            vec3 n = vertices[i].normal;
            vec3 t = vertices[i].tangent;
            vec3 b = bitangents[i];

            // Gram-Schmidt orthogonalize
            t = normalize(t - n * dot(n, t));

            if (dot(cross(n, t), b) < 0.0f)
                t = t * -1.0f;

            vertices[i].tangent = t;
        }
    }

    geometry* geometry::quad()
    {
        if (_quad == nullptr)
        {
            std::vector<vertex> vertices;
            vertices.push_back(vertex(vec3(-0.5f, -0.5f, 0.0f), vec2(0.0f, 0.0f)));
            vertices.push_back(vertex(vec3(0.5f, -0.5f, 0.0f), vec2(1.0f, 0.0f)));
            vertices.push_back(vertex(vec3(0.5f, 0.5f, 0.0f), vec2(1.0f, 1.0f)));
            vertices.push_back(vertex(vec3(-0.5f, 0.5f, 0.0f), vec2(0.0f, 1.0f)));

            auto indices = std::vector<uint>();
            indices.push_back(0);
            indices.push_back(1);
            indices.push_back(2);
            indices.push_back(2);
            indices.push_back(3);
            indices.push_back(0);

            _quad = create(vertices, indices);
        }

        return _quad;
    }
}
