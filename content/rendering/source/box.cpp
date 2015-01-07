#include "box.h"
#include "resourceManager.h"
#include "mesh.h"

namespace phi
{
	box::box(glm::vec3 position, phi::size<float> size, phi::material* material)
		: sceneObject(position, size, material)
	{   
		_mesh = phi::resourceManager::get()->getMesh("box");
	}

	box::~box(void)
	{}
}