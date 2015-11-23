#include "phi/scenes/sphere.h"

namespace phi
{
    sphere::sphere(float radius, int rings, int sectors, material* material)
    {
        _radius = radius;
        _model = new model("sphere", "");
        auto m = create(rings + 1, sectors + 1);
        m->setMaterial(material);
        m->setId(0);
        _model->addMesh(m);
    }

    sphere::~sphere()
    {
    }

    mesh* sphere::create(GLuint rings, GLuint sectors)
    {
        float piOver2 = PI * 0.5f;

        std::vector<vertex> vertices;
        std::vector<GLuint> indices;

        float const R = 1.0f/(float)(rings-1.0f);
        float const S = 1.0f/(float)(sectors-1.0f);
        GLuint r, s;

        for(r = 0; r < rings; r++)
        {
            for(s = 0; s < sectors; s++) 
            {
                float const y = glm::sin(-piOver2 + PI * r * R) * 0.5f;
                float const x = glm::cos(2.0f * PI * s * S) * glm::sin(PI * r * R) * 0.5f;
                float const z = glm::sin(2.0f * PI * s * S) * glm::sin(PI * r * R) * 0.5f;

                glm::vec3 position = glm::vec3(x, y, z);
                glm::vec3 normal = glm::vec3(x,y,z);
                glm::vec2 texCoord = glm::vec2(s * R, r * R);
                vertices.push_back(vertex(position, texCoord, normal));
            }
        }

        for(r = 0; r < rings-1; r++) 
        {
            for(s = 0; s < sectors-1; s++) 
            {
                int a = r * sectors + s;
                int b = (r+1) * sectors + s;
                int c = (r+1) * sectors + (s+1);
                int d = r * sectors + (s+1);

                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(c);
                indices.push_back(d);
                indices.push_back(a);
            }
        }

        mesh::calcTangents(vertices, indices);

        return mesh::create("sphere", vertices, indices);
    }

    void sphere::setRadius(float radius)
    { 
        _radius = radius;
    }
}