#include <precompiled.h>
#include "sphere.h"

namespace phi
{
    sphere::sphere(float radius, phi::geometry* geometry, phi::material* material) :
        mesh("sphere", geometry, material),
        _radius(radius)
    {
    }

    geometry* sphere::createSphereGeometry(uint rings, uint sectors)
    {
        float piOver2 = PI * 0.5f;

        vector<vertex> vertices;
        vector<uint> indices;

        float const R = 1.0f / (float)(rings - 1.0f);
        float const S = 1.0f / (float)(sectors - 1.0f);
        uint r, s;

        for (r = 0; r < rings; r++)
        {
            for (s = 0; s < sectors; s++)
            {
                float const y = sin(-piOver2 + PI * r * R) * 0.5f;
                float const x = cos(2.0f * PI * s * S) * sin(PI * r * R) * 0.5f;
                float const z = sin(2.0f * PI * s * S) * sin(PI * r * R) * 0.5f;

                vec3 position = vec3(x, y, z);
                vec3 normal = vec3(x, y, z);
                vec2 texCoord = vec2(s * R, r * R);
                vertices.push_back(vertex(position, texCoord, normal));
            }
        }

        for (r = 0; r < rings - 1; r++)
        {
            for (s = 0; s < sectors - 1; s++)
            {
                int a = r * sectors + s;
                int b = (r + 1) * sectors + s;
                int c = (r + 1) * sectors + (s + 1);
                int d = r * sectors + (s + 1);

                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(c);
                indices.push_back(d);
                indices.push_back(a);
            }
        }

        geometry::calcTangents(vertices, indices);

        return geometry::create(vertices, indices);
    }

    sphere* sphere::create(float radius, uint rings, uint sectors, phi::material* material)
    {
        auto geometry = createSphereGeometry(rings, sectors);
        return new sphere(radius, geometry, material);
    }

    sphere::~sphere()
    {
    }

    void sphere::setRadius(float radius)
    { 
        _radius = radius;
    }
}