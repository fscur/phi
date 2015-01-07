#include "skyDome.h"
#include "mathUtils.h"

namespace phi
{
    skyDome::skyDome(float radius, int rings, int sectors, material* material)
        : sceneObject(glm::vec3(0.0f), size<float>(radius, radius, radius), material)
    {   
		_radius = radius;
        _mesh = create(rings, sectors);
    }

	skyDome::~skyDome()
    {
    }

	mesh* skyDome::create(unsigned int rings, unsigned int sectors)
	{
		float piOver2 = PI * 0.5f;

		std::vector<vertex> vertices;
		std::vector<GLuint> indices;

		float const R = 1.0f/(float)(rings);
        float const S = 1.0f/(float)(sectors);
        unsigned int r, s;

        for(r = 0; r < rings + 1; r++) 
		{
			for(s = 0; s < sectors + 1; s++) 
			{
                float y = glm::sin(piOver2 * r * R);
                float x = glm::cos(2.0f * PI * s * S) * glm::sin(piOver2 + piOver2 * r * R);
                float z = glm::sin(2.0f * PI * s * S) * glm::sin(piOver2 + piOver2 * r * R);

				glm::vec3 position = glm::vec3(x, y, z);
				glm::vec3 normal = -glm::normalize(glm::vec3(x, y, z));
				glm::vec2 texCoord = glm::vec2(s * R, r * R);
				vertices.push_back(vertex(position, texCoord, normal));
			}
		}
			
        for(r = 0; r < rings; r++) 
		{
			for(s = 0; s < sectors; s++) 
			{
				int a = r * (sectors + 1) + s;
				int b = (r + 1) * (sectors + 1) + s;
				int c = (r + 1) * (sectors + 1) + (s + 1);
				int d = r * (sectors + 1) + (s + 1);

				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(c);

				indices.push_back(c);
				indices.push_back(d);
				indices.push_back(a);
			}
        }

		//Mesh::CalcNormals(vertices, indices);
		mesh::calcTangents(vertices, indices);

		return mesh::create("skyDome", vertices, indices);
	}

	void skyDome::setRadius(float radius)
	{ 
		_radius = radius;
		setSize(size<float>(radius, radius, radius));
	}

	void skyDome::render()
	{
		glCullFace(GL_FRONT);
		sceneObject::render();
		glCullFace(GL_BACK);
	}
}