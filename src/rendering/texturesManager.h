#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "textureContainer.h"
#include "glError.h"

namespace phi
{
    class texturesManager
    {
    private:
        static bool _initialized;

    private:
        const size_t MAX_CONTAINER_ITEMS = 10;
        map<std::tuple<GLsizei, GLsizei, GLsizei, GLenum, GLint, GLint, GLint>, vector<textureContainer*>> _containers;

        bool _bindless;
        bool _sparse;

        GLint _currentTextureUnit;
        GLint _maxTextureUnits;
        size_t _maxContainerSize;

        map<const texture*, textureAddress> _textures;
        
    public:
        vector<GLint> units;
        vector<GLuint64> handles;

    public:
        RENDERING_API texturesManager(bool bindless, bool sparse);
        RENDERING_API ~texturesManager();

        RENDERING_API textureAddress add(const texture* const texture);
        RENDERING_API textureAddress get(const texture* const texture);
        RENDERING_API bool contains(const texture* const texture);

        RENDERING_API void reserveContainer(textureContainerLayout layout, size_t size);
    };
}