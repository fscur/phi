#include <precompiled.h>
#include "random.h"

namespace phi
{
	random* random::global = new random();
	
	inline random::random() :
		_seed(static_cast<uint>(std::time(0)))
	{
		std::srand(_seed);
	}
	
	inline random::random(uint seed) :
		_seed(seed)
	{
		std::srand(_seed);
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