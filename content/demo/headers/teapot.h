#ifndef _TEAPOT_H
#define _TEAPOT_H

#include "sceneObject.h"
#include "material.h"

class teapot :
	public phi::sceneObject
{
public:
	teapot(glm::vec3 position, phi::size<float> size, phi::material* material);
	~teapot(void);
};

#endif