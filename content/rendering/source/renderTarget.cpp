#include "renderTarget.h"
#include "globals.h"

namespace phi
{
	renderTarget::renderTarget(std::string name, texture* texture)
	{
		_name = name;
		_texture = texture;
	}

	renderTarget::~renderTarget()
	{
		_texture->release();
		DELETE(_texture);
	}
}