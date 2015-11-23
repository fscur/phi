#ifndef _PHI_ATTENUATION_H_
#define _PHI_ATTENUATION_H_

#include "rendering.h"

namespace phi
{
	struct attenuation
	{
	public:
		float constant;
		float linear;
		float exponential;

	public:
		attenuation(float constant = 0.0f, float linear = 0.0f, float exponential = 1.0f):
			constant(constant), linear(linear), exponential(exponential) {}

		~attenuation() {}
	};
}
#endif