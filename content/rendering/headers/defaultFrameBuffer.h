#ifndef _PHI_DEFAULT_FRAME_BUFFER_H_
#define _PHI_DEFAULT_FRAME_BUFFER_H_

#include "frameBuffer.h"

namespace phi
{
	class defaultFrameBuffer :
		public frameBuffer
	{
	public:
		RENDERING_API defaultFrameBuffer(size<GLuint> size, color clearColor);
		RENDERING_API ~defaultFrameBuffer();
		RENDERING_API void init() override;
		RENDERING_API void clear() override;
	};
}

#endif