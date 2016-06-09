#include <precompiled.h>

#include <core\invalidInitializationException.h>

#include "texturesManager.h"
#include "bindlessTextureContainer.h"
#include "sparseTextureContainer.h"
#include "sparseBindlessTextureContainer.h"
#include "textureUnits.h"

namespace phi
{
    //TODO: calcular quanto de memoria tem disponivel na GPU
    //TODO: verificar quando de memoria nosso gbuffer + shadow maps usam e ver quanto sobra pra texturas
    //TODO: controlar a memoria da gpu usada

    bool texturesManager::_initialized = false;
    map<image*, texture*> texturesManager::_imageTextures;

    unordered_map<textureLayout, vector<textureContainer*>> texturesManager::_containers;
    map<const texture*, textureAddress> texturesManager::_textures;

    bool texturesManager::_isBindless = false;
    bool texturesManager::_isSparse = false;
    uint texturesManager::_maxPages = texturesManager::DEFAULT_MAX_PAGES;

    vector<GLint> texturesManager::units;
    vector<GLuint64> texturesManager::handles;

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
        for (auto& pair : _containers)
        {
            for (auto container : pair.second)
                safeDelete(container);
        }
    }

    inline uint texturesManager::getMaxLevels(const uint& w, const uint& h)
    {
        auto biggestTextureSize = (float)glm::max(w, h);
        return static_cast<uint>(glm::floor(glm::log2(biggestTextureSize)) + 1.0f);
    }

    void texturesManager::add(const texture* const texture)
    {
        auto layout = texture->layout;
        bool textureIsNotAtlas = !texture->isAtlasTexture;
        if (texture->generateMipmaps && textureIsNotAtlas)
            layout.levels = static_cast<GLsizei>(getMaxLevels(texture->w, texture->h));

        auto textureAddress = phi::textureAddress();
        textureContainer* container;
        sizeui containerSize = sizeui(texture->w, texture->h, _maxPages);

        auto it = _containers.find(layout);
        if (it != _containers.end())
        {
            auto containers = (*it).second;
            uint i = 0;
            bool added = false;
            auto containersCount = containers.size();

            while (!added && i < containersCount)
            {
                container = containers[i++];
                containerSize = container->getSize();
                
                if (textureIsNotAtlas && (containerSize.w != texture->w || containerSize.h != texture->h))
                    continue;

                added = container->add(texture, textureAddress);
            }

            if (added)
            {
                _textures[texture] = textureAddress;
                return;
            }
        }

        container = createContainer(containerSize, layout);
        container->add(texture, textureAddress);
        _textures[texture] = textureAddress;
    }

    textureAddress texturesManager::get(const texture* const texture)
    {
        if (!_initialized)
            throw invalidInitializationException("texturesManager not initialized.");

        if (!contains(texture))
            add(texture);

        return _textures[texture];
    }

    texture* texturesManager::getTextureFromImage(image* materialImage, phi::image* defaultImage)
    {
        phi::texture* texture = nullptr;

        if (materialImage == nullptr)
            materialImage = defaultImage;

        if (_imageTextures.find(materialImage) != _imageTextures.end())
        {
            texture = _imageTextures[materialImage];
        }
        else
        {
            auto layout = textureLayout();
            layout.levels = 0;
            layout.dataFormat = textureLayout::translateDataFormat(materialImage->dataFormat);
            layout.dataType = textureLayout::translateDataType(materialImage->dataType);
            layout.internalFormat = GL_RGBA8;
            layout.wrapMode = GL_REPEAT;
            layout.minFilter = GL_LINEAR_MIPMAP_LINEAR;
            layout.magFilter = GL_LINEAR;

            texture = new phi::texture(
                materialImage,
                layout,
                true,
                false,
                GL_TEXTURE_2D);

            _imageTextures[materialImage] = texture;
        }

        return texture;
    }

    bool texturesManager::contains(const texture* const texture)
    {
        return phi::contains(_textures, texture);
    }

    textureContainer* texturesManager::createContainer(sizeui size, textureLayout layout)
    {
        textureContainer* container;

        if (_isSparse && _isBindless)
            container = new sparseBindlessTextureContainer(size, layout);
        else if (_isSparse)
            container = new sparseTextureContainer(size, layout);
        else if (_isBindless)
            container = new bindlessTextureContainer(size, layout);
        else
            container = new textureContainer(size, layout);

        _containers[layout].push_back(container);
        handles.push_back(container->getHandle());
        units.push_back(container->getUnit());

        return container;
    }

    textureContainer* texturesManager::reserveContainer(sizeui size, textureLayout layout)
    {
        if (!_initialized)
            throw invalidInitializationException("texturesManager not initialized.");

        auto maxPages = std::min(_maxPages, size.d);
        size.d = maxPages;

        return createContainer(size, layout);
    }
}