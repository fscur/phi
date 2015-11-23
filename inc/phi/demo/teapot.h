#ifndef _TEAPOT_H
#define _TEAPOT_H

#include "phi/rendering/material.h"

#include "phi/scenes/sceneObject.h"

class teapot :
	public phi::sceneObject
{
public:
	teapot(glm::vec3 position, phi::size<float> size, phi::material* material);
	~teapot(void);
};

#endif