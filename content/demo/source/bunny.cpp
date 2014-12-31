#include "bunny.h"

bunny::bunny(glm::vec3 position, phi::size<float> size, phi::material* material)
	: sceneObject(position, size, material)
{   
	_mesh = phi::mesh::fromMb("bunny", "bunny.mb");

	//_mesh = Mesh::FromObj("bunny.m", true);
	//_mesh->SaveToMb("C:\\bunny.mb");
}

bunny::~bunny(void)
{}