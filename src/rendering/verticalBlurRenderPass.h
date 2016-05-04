#pragma once
#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "gl.h"
#include "gBufferRenderPass.h"
#include "vertexBuffer.h"
#include "buffer.h"
#include "shader.h"
#include "batch.h"

#include <core\geometry.h>

namespace phi
{
	class verticalBlurRenderPass
	{
	private:

		gl* _gl;
		geometry* _quad;
		shader* _shader;
		vertexBuffer* _quadVbo;
		buffer* _quadEbo;
		float _w;
		float _h;
		uint _quadVao;
		vec2 _resolution;

		framebuffer* _framebuffer;

		renderTarget* _source;
		renderTarget* _result;

	private:
		void createQuad();
		void initShader();
		void initFramebuffer();

	public:
		RENDERING_API verticalBlurRenderPass(renderTarget* source, gl* gl, float w, float h);
		RENDERING_API ~verticalBlurRenderPass();
		
		RENDERING_API renderTarget* getResult() const { return _result; }

		RENDERING_API void update();
		RENDERING_API void render();
	};
}