#pragma once
#include <phi.h>
#include "coreApi.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

namespace phi
{
	class random
	{
	private:
		uint _seed;

	public:
		static random* global;

	public:
		random() :
			_seed(static_cast<uint>(std::time(0)))
		{
			std::cout << "seed:" << std::to_string(_seed) << std::endl;
			std::srand(_seed);
		}
		
		random(uint seed) :
			_seed(seed)
		{
			std::srand(_seed);
		}

		float next()
		{
			return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
		}

		float next(float min, float max)
		{
			float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
			return min + r * (max - min);
		}
	};

	random* random::global = new random();
}