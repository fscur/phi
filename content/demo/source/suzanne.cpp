#include "suzanne.h"

suzanne::suzanne(glm::vec3 position, phi::size<float> size, phi::material* material)
	: sceneObject(position, size, material)
{   
	_mesh =  phi::mesh::fromMb("suzanne_high.mb");

	//_mesh = Mesh::FromObj("suzanne_high.m", true);
	//_mesh->SaveToMb("C:\\suzanne_high.mb");
}

suzanne::~suzanne(void)
{}