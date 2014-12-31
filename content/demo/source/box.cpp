#include "box.h"
#include "mesh.h"

box::box(glm::vec3 position, phi::size<float> size, phi::material* material)
	: sceneObject(position, size, material)
{   
	_mesh = phi::rendering::repository->getResource<phi::mesh>("box");
}

box::~box(void)
{}