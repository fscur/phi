#include "phi/demo/teapot.h"

teapot::teapot(glm::vec3 position, phi::size<float> size, phi::material* material)
	: phi::sceneObject()
{   
	//_mesh = phi::mesh::fromMb("teapot", "teapot.mb");

	//_mesh = Mesh::FromObj("teapot.m", true);
	//_mesh->SaveToMb("C:\\teapot.mb");
}

teapot::~teapot(void)
{}