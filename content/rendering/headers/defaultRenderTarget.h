#ifndef _PHI_DEFAULT_RENDER_TARGET_H_
#define _PHI_DEFAULT_RENDER_TARGET_H_

#include "renderTarget.h"

namespace phi
{
    class defaultRenderTarget :
        public renderTarget
    {
    public:
        RENDERING_API defaultRenderTarget(size<GLuint> size, color backColor);
        RENDERING_API ~defaultRenderTarget();
        RENDERING_API bool init() override;
        RENDERING_API void clear() override;
    };
}

#endif