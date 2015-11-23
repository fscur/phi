#ifndef _PHI_PICKING_FRAME_BUFFER_H_
#define _PHI_PICKING_FRAME_BUFFER_H_

#include "frameBuffer.h"

namespace phi
{
	class pickingFrameBuffer :
		public frameBuffer
	{
	private:
		texture* _pickingTexture;
	public:
		RENDERING_API pickingFrameBuffer(size<GLuint> size);
		RENDERING_API ~pickingFrameBuffer();
		
		RENDERING_API texture* getPickingTexture() const { return _pickingTexture; }
		
		RENDERING_API void init() override;
		RENDERING_API void clear() override;

		RENDERING_API GLuint pick(glm::vec2 mousePos);
	};
}

#endif