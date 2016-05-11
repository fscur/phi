#pragma once
#include <phi.h>
#include "coreApi.h"

namespace phi
{
	class random
	{
	private:
		uint _seed;

	public:
		CORE_API static random* global;

	public:
		CORE_API random();
		CORE_API random(uint seed);

		CORE_API float next();
		CORE_API float next(float min, float max);
	};
}