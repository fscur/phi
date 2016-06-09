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
        static const uint DEFAULT_MAX_PAGES = 10;

        static map<image*, texture*> _imageTextures;
        static unordered_map<textureLayout, vector<textureContainer*>> _containers;
        static map<const texture*, textureAddress> _textures;

        static bool _initialized;
        static bool _isBindless;
        static bool _isSparse;
        static uint _maxPages;

    public:
        RENDERING_API static vector<GLint> units;
        RENDERING_API static vector<GLuint64> handles;

    private:
        static void add(const texture* const texture);
        static bool contains(const texture* const texture);
        static textureContainer * createContainer(sizeui size, textureLayout layout);

    public:
        RENDERING_API static void initialize(bool sparse, bool bindless);
        RENDERING_API static void release();
        RENDERING_API static uint getMaxLevels(const uint& w, const uint& h);
        RENDERING_API static texture* getTextureFromImage(image* image);
        RENDERING_API static textureAddress get(const texture* const texture);
        RENDERING_API static textureContainer* reserveContainer(sizeui size, textureLayout layout);
        RENDERING_API static bool getIsBindless() { return _isBindless; }
        RENDERING_API static bool getIsSparse() { return _isSparse; }
    };
}