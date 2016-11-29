#include <precompiled.h>

#include <core\invalidInitializationException.h>
#include <core\keyNotFoundException.h>

#include "texturesManager.h"
#include "bindlessTextureArray.h"
#include "sparseTextureArray.h"
#include "sparseBindlessTextureArray.h"
#include "textureUnits.h"

namespace phi
{
    //TODO: calcular quanto de memoria tem disponivel na GPU
    //TODO: verificar quando de memoria nosso gbuffer + shadow maps usam e ver quanto sobra pra texturas
    //TODO: controlar a memoria da gpu usada

    bool texturesManager::_initialized = false;
    map<image*, texture*> texturesManager::_imageTextures;

    unordered_map<textureLayout, vector<textureArray*>> texturesManager::_textureArrays;
    unordered_map<textureLayout, vector<cubeMapArray*>> texturesManager::_cubeMapArrays;
    
    map<const texture*, textureAddress> texturesManager::_textures;
    map<const cubeMap*, textureAddress> texturesManager::_cubeMaps;

    bool texturesManager::_isBindless = false;
    bool texturesManager::_isSparse = false;
    uint texturesManager::_maxPages = texturesManager::DEFAULT_MAX_PAGES;

    vector<GLint> texturesManager::textureArraysUnits;
    vector<GLint> texturesManager::cubeMapArraysUnits;
    vector<GLuint64> texturesManager::textureArraysHandles;
    vector<GLuint64> texturesManager::cubeMapArraysHandles;

    void texturesManager::initialize(bool sparse, bool bindless)
    {
        if (_initialized)
            throw invalidInitializationException("texturesManager is already initialized.");

        _initialized = true;

        _isSparse = sparse;
        _isBindless = bindless;
        if (_isSparse)
        {
            GLint maxPages;
            glGetIntegerv(GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS, &maxPages);
            _maxPages = static_cast<size_t>(maxPages);
        }

        textureUnits::initialize();
    }

    void texturesManager::release()
    {
        for (auto& pair : _textureArrays)
        {
            for (auto& textureArray : pair.second)
                safeDelete(textureArray);
        }

        for (auto& pair : _cubeMapArrays)
        {
            for (auto& cubeMapArray : pair.second)
                safeDelete(cubeMapArray);
        }
    }

    inline uint texturesManager::getMaxLevels(const uint& w, const uint& h)
    {
        auto biggestTextureSize = (float)glm::max(w, h);
        return static_cast<uint>(glm::floor(glm::log2(biggestTextureSize)) + 1.0f);
    }

    void texturesManager::removeTexture(texture* texture)
    {
        auto address = getTextureAddress(texture);

        auto textureArray = findTextureArray(address.arrayId);

        textureArray->remove(texture);

        if (textureArray->isEmpty())
        {
            if (_isBindless)
            {
                auto handle = textureArray->getHandle();
                phi::removeIfContains(textureArraysHandles, handle);
            }

            textureArray->release();
            removeTextureArray(textureArray);
            safeDelete(textureArray);
        }
    }

    void texturesManager::removeCubeMap(cubeMap* cubeMap)
    {
        auto address = getCubeMapAddress(cubeMap);

        auto cubeMapArray = findCubeMapArray(address.arrayId);

        cubeMapArray->remove(cubeMap);

        if (cubeMapArray->isEmpty())
        {
            if (_isBindless)
            {
                auto handle = cubeMapArray->getHandle();
                phi::removeIfContains(cubeMapArraysHandles, handle);
            }

            cubeMapArray->release();
            removeCubeMapArray(cubeMapArray);
            safeDelete(cubeMapArray);
        }
    }

    textureAddress texturesManager::addTexture(const texture* const texture)
    {   
        auto layout = texture->layout;

        auto textureAddress = phi::textureAddress();
        textureArray* textureArray;
        sizeui size = sizeui(texture->w, texture->h, _maxPages);
        auto index = static_cast<int>(textureArraysUnits.size());

        auto it = _textureArrays.find(layout);
        if (it != _textureArrays.end())
        {
            auto arrays = (*it).second;
            uint i = 0;
            bool added = false;
            auto arraysCount = arrays.size();
            sizeui arraySize = sizeui(0);
            while (!added && i < arraysCount)
            {
                textureArray = arrays[i++];
                arraySize = textureArray->getSize();
                
                if (arraySize.w != texture->w || arraySize.h != texture->h)
                    continue;

                added = textureArray->add(texture, textureAddress);
            }

            if (added)
            {
                index = static_cast<int>(phi::indexOf(textureArraysUnits, textureAddress.unit));
                textureAddress.index = index;
                _textures[texture] = textureAddress;
                return textureAddress;
            }
        }

        textureArray = createTextureArray(size, layout);
        textureAddress.index = static_cast<int>(textureArraysUnits.size() - 1);
        textureArray->add(texture, textureAddress);
        _textures[texture] = textureAddress;

        return textureAddress;
    }

    textureAddress texturesManager::addTextureAsAtlasTexture(const texture* const texture)
    {
        auto textureAddress = phi::textureAddress();
        textureArray* textureArray;
        sizeui arraySize = sizeui(texture->w, texture->h, _maxPages);
        auto layout = texture->layout;
        auto index = static_cast<int>(textureArraysUnits.size());
        auto it = _textureArrays.find(layout);
        if (it != _textureArrays.end())
        {
            auto arrays = (*it).second;
            uint i = 0;
            bool added = false;
            auto arraysCount = arrays.size();

            while (!added && i < arraysCount)
            {
                textureArray = arrays[i++];
                arraySize = textureArray->getSize();

                added = textureArray->add(texture, textureAddress);
            }

            if (added)
            {
                index = static_cast<int>(phi::indexOf(textureArraysUnits, textureAddress.unit));
                textureAddress.index = index;
                _textures[texture] = textureAddress;
                return textureAddress;
            }
        }

        textureArray = createTextureArray(arraySize, layout);
        textureAddress.index = static_cast<int>(textureArraysUnits.size() - 1);
        textureArray->add(texture, textureAddress);
        _textures[texture] = textureAddress;

        return textureAddress;
    }

    textureAddress texturesManager::addCubeMap(const cubeMap* const cubeMap)
    {
        auto layout = cubeMap->layout;

        auto cubeMapAddress = phi::textureAddress();
        cubeMapArray* cubeMapArray;
        sizeui size = sizeui(cubeMap->w, cubeMap->h, _maxPages);

        auto it = _cubeMapArrays.find(layout);
        if (it != _cubeMapArrays.end())
        {
            auto arrays = (*it).second;
            uint i = 0;
            bool added = false;
            auto arraysCount = arrays.size();
            sizeui arraySize = sizeui(0);

            while (!added && i < arraysCount)
            {
                cubeMapArray = arrays[i++];
                arraySize = cubeMapArray->getSize();

                if (arraySize.w != cubeMap->w || arraySize.h != cubeMap->h)
                    continue;

                added = cubeMapArray->add(cubeMap, cubeMapAddress);
            }

            if (added)
            {
                auto index = static_cast<int>(phi::indexOf(cubeMapArraysUnits, cubeMapAddress.unit));
                cubeMapAddress.index = index;
                _cubeMaps[cubeMap] = cubeMapAddress;
                return cubeMapAddress;
            }
        }

        cubeMapArray = createCubeMapArray(size, layout);
        cubeMapAddress.index = static_cast<int>(cubeMapArraysUnits.size() - 1);
        cubeMapArray->add(cubeMap, cubeMapAddress);
        _cubeMaps[cubeMap] = cubeMapAddress;

        return cubeMapAddress;
    }
    
    cubeMapArray* texturesManager::createCubeMapArray(sizeui size, textureLayout layout)
    {
        auto cubeMapArray = new phi::cubeMapArray(size, layout);
        cubeMapArraysUnits.push_back(cubeMapArray->getUnit());
        _cubeMapArrays[layout].push_back(cubeMapArray);



        //if (_isBindless)
          //  textureArraysHandles.push_back(textureArray->getHandle());

        return cubeMapArray;
    }

    textureAddress texturesManager::getTextureAddress(const texture* const texture)
    {
        if (!_initialized)
            throw invalidInitializationException("texturesManager not initialized.");

        if (contains(texture))
            return _textures[texture];

        throw keyNotFoundException("texture not found.");
    }

    textureAddress texturesManager::getCubeMapAddress(const cubeMap* const cubeMap)
    {
        if (!_initialized)
            throw invalidInitializationException("texturesManager not initialized.");

        if (contains(cubeMap))
            return _cubeMaps[cubeMap];

        throw keyNotFoundException("texture not found.");
    }

    texture* texturesManager::getTextureFromImage(phi::image* image, bool generateMipmaps)
    {
        if (!image)
            throw argumentException("image cant be nullptr.");

        phi::texture* texture = nullptr;

        if (_imageTextures.find(image) != _imageTextures.end())
        {
            return _imageTextures[image];
        }
        else
        {
            auto layout = textureLayout();
            layout.dataFormat = textureLayout::translateDataFormat(image->dataFormat);
            layout.dataType = textureLayout::translateDataType(image->dataType);
            layout.internalFormat = GL_RGBA8;
            layout.wrapMode = GL_REPEAT;
            layout.minFilter = GL_LINEAR_MIPMAP_LINEAR;
            layout.magFilter = GL_LINEAR;
            layout.levels = generateMipmaps ? static_cast<GLsizei>(getMaxLevels(image->w, image->h)) : 1;

            texture = new phi::texture(
                image,
                layout,
                false,
                GL_TEXTURE_2D);

            _imageTextures[image] = texture;
            return texture;
        }
    }

    bool texturesManager::contains(const texture* const texture)
    {
        return phi::contains(_textures, texture);
    }

    bool texturesManager::contains(const cubeMap* const cubeMap)
    {
        return phi::contains(_cubeMaps, cubeMap);
    }

    textureArray* texturesManager::createTextureArray(sizeui size, textureLayout layout)
    {
        textureArray* textureArray;

        if (_isSparse && _isBindless)
            textureArray = new sparseBindlessTextureArray(size, layout);
        else if (_isSparse)
            textureArray = new sparseTextureArray(size, layout);
        else if (_isBindless)
            textureArray = new bindlessTextureArray(size, layout);
        else
            textureArray = new phi::textureArray(size, layout);

        _textureArrays[layout].push_back(textureArray);

        if (_isBindless)
            textureArraysHandles.push_back(textureArray->getHandle());
        else
            textureArraysUnits.push_back(textureArray->getUnit());

        return textureArray;
    }

    textureArray* texturesManager::findTextureArray(const GLuint id)
    {
        for (auto& pair : _textureArrays)
        {
            auto arrays = pair.second;

            auto it = std::find_if(
                arrays.begin(),
                arrays.end(),
                [=](textureArray* textureArray)
                { 
                    return textureArray->getId() == id;
                }
            );

            if (it != arrays.end())
                return *it;
        }

        throw argumentException(id + " texture array Id not found.");
    }

    cubeMapArray* texturesManager::findCubeMapArray(const GLuint id)
    {
        for (auto& pair : _cubeMapArrays)
        {
            auto arrays = pair.second;

            auto it = std::find_if(
                arrays.begin(),
                arrays.end(),
                [=](cubeMapArray* cubeMapArray)
            {
                return cubeMapArray->getId() == id;
            }
            );

            if (it != arrays.end())
                return *it;
        }

        throw argumentException(id + " cubeMap Array Id not found.");
    }

    void texturesManager::removeTextureArray(textureArray* textureArray)
    {
        phi::removeIfContains(_textureArrays[textureArray->getLayout()], textureArray);
    }

    void texturesManager::removeCubeMapArray(cubeMapArray* cubeMapArray)
    {
        phi::removeIfContains(_cubeMapArrays[cubeMapArray->getLayout()], cubeMapArray);
    }

    textureArray* texturesManager::reserveTextureArray(sizeui size, textureLayout layout)
    {
        if (!_initialized)
            throw invalidInitializationException("texturesManager not initialized.");

        auto maxPages = std::min(_maxPages, size.d);
        size.d = maxPages;

        return createTextureArray(size, layout);
    }

    cubeMapArray* texturesManager::reserveCubeMapArray(sizeui size, textureLayout layout)
    {
        if (!_initialized)
            throw invalidInitializationException("texturesManager not initialized.");

        auto maxPages = std::min(_maxPages, size.d);
        size.d = maxPages;

        return createCubeMapArray(size, layout);
    }

    bool texturesManager::getIsBindless() { return _isBindless; }
    bool texturesManager::getIsSparse() { return _isSparse; }
}