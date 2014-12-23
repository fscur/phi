#ifndef _PHI_TRANSFORM_H_
#define _PHI_TRANSFORM_H_

#include "rendering.h"
#include <glm\glm.hpp>

namespace phi
{
    struct transform
    {
    private:
        glm::mat4 _p;
        glm::mat4 _v;
        glm::mat4 _m;
    public:
        RENDERING_API transform();
        RENDERING_API ~transform();
        
        RENDERING_API glm::mat4 getProjectionMatrix() const { return _p; }
        RENDERING_API glm::mat4 getViewMatrix() const { return  _v; }
        RENDERING_API glm::mat4 getModelMatrix() const { return _m; }
		RENDERING_API glm::mat4 getMv() const { return _v * _m; }
		RENDERING_API glm::mat4 getMvp() const { return _p * _v * _m; }
		RENDERING_API glm::mat4 getItmv() const { return glm::inverse(glm::transpose(_v * _m)); }
        
        RENDERING_API void setProjectionMatrix(glm::mat4 value) { _p = value; }
        RENDERING_API void setViewMatrix(glm::mat4 value) { _v = value; }
        RENDERING_API void setModelMatrix(glm::mat4 value) { _m = value; }
    };
}

#endif