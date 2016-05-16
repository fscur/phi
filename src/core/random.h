#pragma once
#include <phi.h>
#include "coreApi.h"

namespace phi
{
	class random
	{	
	private:
		static random _global;
		random();

	public:
		CORE_API static float next();
		CORE_API static float next(float min, float max);
	};
}