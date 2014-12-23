#ifndef _SUZANNE_H
#define _SUZANNE_H

#include "sceneObject.h"
#include "material.h"

class suzanne :
	public phi::sceneObject
{
public:
	suzanne(glm::vec3 position, phi::size<float> size, phi::material* material);
	~suzanne(void);
};

#endif