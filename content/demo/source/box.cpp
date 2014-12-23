#include "box.h"
#include "resourceManager.h"
#include "mesh.h"

box::box(glm::vec3 position, phi::size<float> size, phi::material* material)
	: sceneObject(position, size, material)
{   
	_mesh = phi::resourceManager::get()->getMesh("BOX");

	if (_mesh == NULL)
	{
		_mesh = phi::mesh::fromObj("cube.model");
		// _mesh = Mesh::FromObj("cube_smooth.m", true);
		_mesh->saveToMb("C:\\cube.mb");

		//_mesh = phi::mesh::fromMb("cube.mb");
		//phi::resourceManager::get()->addMesh("BOX", _mesh);
	}
}

box::~box(void)
{}