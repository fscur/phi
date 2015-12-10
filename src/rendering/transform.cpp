#include <phi/rendering/transform.h>

namespace phi
{
    transform::transform(): 
		_p(mat4(1.0f)), 
		_v(mat4(1.0f)), 
		_m(mat4(1.0f))
    {
    }

    transform::~transform()
    {
    }
}