#ifndef _PHI_BOX_H
#define _PHI_BOX_H

#include "sceneObject.h"
#include "material.h"

namespace phi
{
	class box :
		public sceneObject
	{
	public:
		SCENES_API box(glm::vec3 position, phi::size<float> size, phi::material* material);
		SCENES_API ~box(void);
	};
}

#endif