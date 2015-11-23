#include "phi/demo/bunny.h"

bunny::bunny(glm::vec3 position, phi::size<float> size, phi::material* material)
	: sceneObject()
{   
	//_mesh = phi::mesh::fromMesh("resources/models/test0.mesh");

	//_mesh = Mesh::FromObj("bunny.m", true);
	//_mesh->SaveToMb("C:\\bunny.mb");
}

bunny::~bunny(void)
{}