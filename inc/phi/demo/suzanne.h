#ifndef _SUZANNE_H
#define _SUZANNE_H


#include "phi/rendering/material.h"

#include "phi/scenes/sceneObject.h"

class suzanne :
	public phi::sceneObject
{
public:
	suzanne(glm::vec3 position, phi::size<float> size, phi::material* material);
	~suzanne(void);
};

#endif