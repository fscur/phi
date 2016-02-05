#include <phi\scenes\cone.h>

namespace phi
{
    cone::cone(phi::geometry* geometry, phi::material* material) :
        mesh("cone", geometry, material)
    {}

    cone::~cone()
    {
    }

    geometry* cone::createConeGeometry(uint sectors)
    {
        //TODO: bug-> When creating a cone with 100 sides or more, the geometry is fucked up!!!!!
        std::vector<vertex> tempVertices;

        vec3 top = vec3(0.0f, 0.0f, 0.0f);
        vec3 bot = vec3(0.0f, 0.0f, 1.0f);

        float radius = 0.5f;
        int t = sectors;
        int verts = 6;

        double increment = 1.0f / ((double)t);

        for (int i = 0; i < t * verts; i += verts)
        {
            double u = increment * (double)i / (double)verts;
            double v = u + increment;

            double angle0 = u * PI * 2.0;
            double angle1 = v * PI * 2.0;

            double x0 = cos(-angle0) * radius;
            double y0 = sin(-angle0) * radius;
            double x1 = cos(-angle1) * radius;
            double y1 = sin(-angle1) * radius;

            vec3 a = vec3(x0, y0, 1.0f);
            vec3 b = vec3(x1, y1, 1.0f);

            vec3 v0 = a - top;
            vec3 v1 = b - top;

            vec3 normal = normalize(cross(v0, v1));

            tempVertices.push_back(vertex(top, vec2((u + v) / 2.0f, 1.0f), normal));
            tempVertices.push_back(vertex(a, vec2(u, 0.0f), normal));
            tempVertices.push_back(vertex(b, vec2(v, 0.0f), normal));

            a = vec3(x1, y1, 1.0f);
            b = vec3(x0, y0, 1.0f);

            v0 = a - bot;
            v1 = b - bot;

            normal = normalize(cross(v0, v1));

            tempVertices.push_back(vertex(bot, vec2(0.5f, 0.5f), normal));
            tempVertices.push_back(vertex(a, vec2(u, 0.0f), normal));
            tempVertices.push_back(vertex(b, vec2(v, 0.0f), normal));
        }

        std::vector<vec3> normals;

        for (int i = 0; i < t * verts; i += verts)
        {
            int j = i == 0 ? (t - 1) * verts : i - verts;

            vertex* v0 = &tempVertices[i];
            vertex* v1 = &tempVertices[j];

            vec3 n0 = v0->normal;
            vec3 n1 = v1->normal;
            vec3 n = normalize(n0 + n1);

            normals.push_back(n);
        }

        for (int i = 0; i < t * verts; i += verts)
        {
            int j = i == 0 ? (t - 1) * verts : i - verts;
            int k = i / verts;

            vertex* v = &tempVertices[i];
            v->normal = normals[k];

            v = &tempVertices[i + 1];
            v->normal = normals[k];

            v = &tempVertices[j + 2];
            v->normal = normals[k];
        }

        //select vertices and create indices
        std::vector<GLuint> indices;
        std::vector<vertex> vertices;
        unsigned int vSize = (unsigned int)tempVertices.size();

        vertex vertex;
        bool exists = false;
        int index = 0;
        unsigned int i = 0;
        unsigned int j = 0;
        unsigned int verticesSize = 0;

        for (i = 0; i < vSize; i++)
        {
            vertex = tempVertices[i];
            exists = false;
            index = 0;

            for (j = 0; j < verticesSize; j++)
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
        
        phi::geometry::calcTangents(vertices, indices);

        return phi::geometry::create(vertices, indices);
    }

    cone* cone::create(uint sectors, phi::material* material)
    {
        auto geometry = createConeGeometry(sectors);
        return new cone(geometry, material);
    }
}