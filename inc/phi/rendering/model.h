#ifndef _PHI_MODEL_H_
#define _PHI_MODEL_H_

#include <phi/core/object3D.h>

#include "rendering.h"
#include "texture.h"

namespace phi
{
	class model :
        public object3D
	{
    private:
        texture* _thumbnail;
    public:
        RENDERING_API model(std::string name);
        RENDERING_API texture* getThumbnail() const { return _thumbnail; }
        RENDERING_API void setThumbnail(texture* value) { _thumbnail = value; }
        RENDERING_API static model* fromFile(std::string fileName);
    };
}
#endif