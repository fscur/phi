#ifndef _BOX_H
#define _BOX_H

#include "sceneObject.h"
#include "material.h"

class box :
	public phi::sceneObject
{
public:
	box(glm::vec3 position, phi::size<float> size, phi::material* material);
	~box(void);
};

#endif