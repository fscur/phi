#ifndef _PHI_TRANSFORM_H_
#define _PHI_TRANSFORM_H_

#include "rendering.h"

#include <phi/core/globals.h>

namespace phi
{
    struct transform
    {
    private:
        mat4 _p;
        mat4 _v;
        mat4 _m;
    public:
        RENDERING_API transform();
        RENDERING_API ~transform();

        RENDERING_API mat4 getProjectionMatrix() const { return _p; }
        RENDERING_API mat4 getViewMatrix() const { return  _v; }
        RENDERING_API mat4 getModelMatrix() const { return _m; }
        RENDERING_API mat4 getMv() const { return _v * _m; }
        RENDERING_API mat4 getMvp() const { return _p * _v * _m; }
        RENDERING_API mat4 getItmv() const { return inverse(transpose(_v * _m)); }

        RENDERING_API void setProjectionMatrix(mat4 value) { _p = value; }
        RENDERING_API void setViewMatrix(mat4 value) { _v = value; }
        RENDERING_API void setModelMatrix(mat4 value) { _m = value; }
    };
}

#endif