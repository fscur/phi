#ifndef _PHI_TEXTURE_ARRAY_H_
#define _PHI_TEXTURE_ARRAY_H_

#include <phi\core\size.h>

#include "rendering.h"
#include "texture.h"

#include <map>

namespace phi
{
    class textureArray
    {
    private:
        const uint MAX_MIPMAP_LEVELS_ALLOWED = 8;

        std::map<texture*, int> _textures;
        bool _isLoadedOnGpu;

    public:
        GLuint id;
        GLuint64 handle;
        GLint textureUnit;
        GLuint w;
        GLuint h;
        GLuint texCount;

    public:
        RENDERING_API textureArray(GLuint w, GLuint h, GLuint texCount, GLint textureUnit) :
            w(w), 
            h(h),
            texCount(texCount),
            textureUnit(textureUnit), 
            _isLoadedOnGpu(false) 
        {}

        RENDERING_API void add(texture* tex);
        RENDERING_API void remove(texture* tex);
        RENDERING_API bool hasTexture(texture* tex);
        RENDERING_API int getTextureIndex(texture* tex);
        RENDERING_API void loadOnGpu();
        RENDERING_API void releaseFromGpu();
    };
}

#endif