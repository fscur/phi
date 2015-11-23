#ifndef _BUNNY_H
#define _BUNNY_H

#include "phi/rendering/material.h"

#include "phi/scenes/sceneObject.h"

class bunny :
	public phi::sceneObject
{
public:
	bunny(glm::vec3 position, phi::size<float> size, phi::material* material);
	~bunny(void);
};

#endif