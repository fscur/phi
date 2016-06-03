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
        static map<image*, texture*> _imageTextures;

    private:
        const size_t MAX_CONTAINER_PAGES = 10;
        unordered_map<textureLayout, vector<textureContainer*>> _containers;

        bool _bindless;
        bool _sparse;

        size_t _maxPages;

        map<const texture*, textureAddress> _textures;
    public:
        vector<GLint> units;
        vector<GLuint64> handles;

    private:
        void add(const texture* const texture);

    public:
        RENDERING_API static uint getMaxLevels(const uint& w, const uint& h);
        RENDERING_API static texture* getTextureFromImage(image* image, phi::image* defaultImage = nullptr);

    public:
        RENDERING_API texturesManager(bool bindless, bool sparse);
        RENDERING_API ~texturesManager();

        RENDERING_API textureAddress get(const texture* const texture);
        RENDERING_API bool contains(const texture* const texture);
        RENDERING_API textureContainer* reserveContainer(sizeui size, textureLayout layout, size_t pages);
    };
}