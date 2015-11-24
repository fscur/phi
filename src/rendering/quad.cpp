#include <phi/rendering/quad.h>

namespace phi
{
	quad::quad() :
		mesh()
	{
		std::vector<vertex> vertices;
		vertices.push_back(vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f)));
		vertices.push_back(vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f)));
		vertices.push_back(vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(1.0f, 1.0f)));
		vertices.push_back(vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 1.0f)));

		std::vector<GLuint> indices;
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(3);
		indices.push_back(0);

		addVertices(vertices, indices);
	}

	quad::~quad()
	{
	}
}