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
        const uint MAX_CONTAINER_PAGES = 10;
        unordered_map<textureLayout, vector<textureContainer*>> _containers;
        map<const texture*, textureAddress> _textures;

        bool _bindless;
        bool _sparse;

        uint _maxPages;

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
        RENDERING_API textureContainer* reserveContainer(sizeui size, textureLayout layout);
    };
}