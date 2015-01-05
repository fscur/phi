#include "box.h"
#include "mesh.h"
#include "renderingSystem.h"

box::box(glm::vec3 position, phi::size<float> size, phi::material* material)
	: sceneObject(position, size, material)
{   
	_mesh = phi::renderingSystem::repository->getResource<phi::mesh>("box");
}

box::~box(void)
{}