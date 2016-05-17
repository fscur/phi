#include <precompiled.h>
#include "random.h"

namespace phi
{
	phi::random random::_global = phi::random();
	
	inline random::random()
	{
		std::srand(static_cast<uint>(std::time(0)));
	}
	
	inline float random::next()
	{
		return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
	}

	inline float random::next(float min, float max)
	{
		float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
		return min + r * (max - min);
	}
}