#ifndef _PHI_MATH_UTILS_H_
#define _PHI_MATH_UTILS_H_

#include "core.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <list>

namespace phi
{
    class mathUtils
    {
    public:
		
		static CORE_API float max(float a, float b)
		{
			if (a > b)
				return a;
			else
				return b;
		}

		static CORE_API float round(float value, int decimals)
		{
			float exp = pow(10.0f, (float)decimals);
			value *= exp;

			int istack = (int)floor(value);
			float out = value - istack;

			if (out < 0.5)
			{
				value = floor(value);
				value /= exp;
				return value;
			}

			if (out > 0.4) 
			{
				value = ceil(value);
				value /= exp;
				return value;
			}

			return value;
		}

        static CORE_API float distance(glm::vec3 v0, glm::vec3 v1);
        static CORE_API std::list<glm::vec3> rotateAboutAxis(std::list<glm::vec3>* points, glm::vec3 origin, glm::vec3 axis, float angle);
        static CORE_API std::list<glm::vec3> rotateAboutAxis(std::list<glm::vec3>* points, glm::vec3 axis, float angle);
        static CORE_API glm::vec3 rotateAboutAxis(glm::vec3 point, glm::vec3 origin, glm::vec3 axis, float angle);
        static CORE_API glm::vec3 rotateAboutAxis(glm::vec3 point, glm::vec3 axis, float angle);
        static CORE_API glm::mat4 getRotationMatrixAboutAnArbitraryAxis(glm::vec3 origin, glm::vec3 axis, float angle);
        static CORE_API glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
    };
}
#endif
