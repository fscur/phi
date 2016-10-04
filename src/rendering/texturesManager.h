#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "textureArray.h"
#include "cubeMapArray.h"

namespace phi
{
    class texturesManager
    {
    public:
        RENDERING_API static void initialize(bool sparse, bool bindless);
        RENDERING_API static void release();
        RENDERING_API static uint getMaxLevels(const uint& w, const uint& h);
        RENDERING_API static bool contains(const texture* const texture);
        RENDERING_API static bool contains(const cubeMap * const cubeMap);
        RENDERING_API static textureAddress addTexture(const texture* const texture);
        RENDERING_API static textureAddress addTextureAsAtlasTexture(const texture* const texture);
        RENDERING_API static textureAddress addCubeMap(const cubeMap* const cubeMap);
        RENDERING_API static void removeTexture(texture* texture);
        RENDERING_API static void removeCubeMap(cubeMap * cubeMap);
        RENDERING_API static texture* getTextureFromImage(image* image, const bool generateMipmaps = true);
        RENDERING_API static textureAddress getTextureAddress(const texture* const texture);
        RENDERING_API static textureAddress getCubeMapAddress(const cubeMap * const cubeMap);
        RENDERING_API static textureArray* reserveTextureArray(sizeui size, textureLayout layout);
        RENDERING_API static cubeMapArray * reserveCubeMapArray(sizeui size, textureLayout layout);
        RENDERING_API static bool getIsBindless();
        RENDERING_API static bool getIsSparse();

    private:
        static textureArray* createTextureArray(sizeui size, textureLayout layout);
        static cubeMapArray* createCubeMapArray(sizeui size, textureLayout layout);
        static textureArray* findTextureArray(const GLuint arrayId);
        static cubeMapArray* findCubeMapArray(const GLuint id);
        static void removeTextureArray(textureArray* textureArray);
        static void removeCubeMapArray(cubeMapArray* cubeMapArray);

    public:
        RENDERING_API static vector<GLint> textureArraysUnits;
        RENDERING_API static vector<GLint> cubeMapArraysUnits;
        RENDERING_API static vector<GLuint64> textureArraysHandles;
        RENDERING_API static vector<GLuint64> cubeMapArraysHandles;

    private:
        static const uint DEFAULT_MAX_PAGES = 10;

        static map<image*, texture*> _imageTextures;
        static unordered_map<textureLayout, vector<textureArray*>> _textureArrays;
        static unordered_map<textureLayout, vector<cubeMapArray*>> _cubeMapArrays;

        static map<const texture*, textureAddress> _textures;
        static map<const cubeMap*, textureAddress> _cubeMaps;

        static bool _initialized;
        static bool _isBindless;
        static bool _isSparse;
        static uint _maxPages;
    };
}