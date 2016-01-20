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

        sizeui _size;
        std::map<texture*, int> _textures;
        bool _isLoadedOnGpu;
        GLuint _id;

    public:
        RENDERING_API textureArray(sizeui size);

        RENDERING_API inline GLuint getId() const { return _id; }
        RENDERING_API inline sizeui getSize() const { return _size; }

        RENDERING_API void add(texture* tex);
        RENDERING_API int getTextureIndex(texture* tex);
        RENDERING_API void loadOnGpu();
        RENDERING_API void releaseFromGpu();
    };
}

#endif