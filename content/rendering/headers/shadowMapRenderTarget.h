#ifndef _PHI_SHADOWMAP_RENDER_TARGET_H_
#define _PHI_SHADOWMAP_RENDER_TARGET_H_

#include "renderTarget.h"
#include "texture.h"

namespace phi
{
    class shadowMapRenderTarget :
        public renderTarget
    {
    private:
        texture* _shadowMap;
    public:
		RENDERING_API shadowMapRenderTarget(GLuint width, GLuint height);
        RENDERING_API ~shadowMapRenderTarget();
        RENDERING_API bool init() override;
        RENDERING_API void bind() override;
        RENDERING_API void clear() override;
        RENDERING_API texture* getShadowMap();
    };
}

#endif