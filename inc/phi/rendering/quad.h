#ifndef _PHI_QUAD_H_
#define _PHI_QUAD_H_

#include "mesh.h"

namespace phi
{
	class quad :
		public mesh
	{
	public:
		RENDERING_API quad();
		RENDERING_API ~quad();
	};
}

#endif