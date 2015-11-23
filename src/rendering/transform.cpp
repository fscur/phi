#include "phi/rendering/transform.h"

namespace phi
{
    transform::transform(): 
		_p(glm::mat4(1.0f)), 
		_v(glm::mat4(1.0f)), 
		_m(glm::mat4(1.0f))
    {
    }

    transform::~transform()
    {
    }
}