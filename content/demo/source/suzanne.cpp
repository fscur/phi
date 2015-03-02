#include "suzanne.h"

suzanne::suzanne(glm::vec3 position, phi::size<float> size, phi::material* material)
	: sceneObject()
{   
	//_mesh =  phi::mesh::fromModel("suzanne_high", "suzanne_high.mb");

	//_mesh = Mesh::FromObj("suzanne_high.m", true);
	//_mesh->SaveToMb("C:\\suzanne_high.mb");
}

suzanne::~suzanne(void)
{}