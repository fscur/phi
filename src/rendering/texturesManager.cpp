#include <precompiled.h>

#include <core\invalidInitializationException.h>
#include <core\keyNotFoundException.h>

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

    textureAddress texturesManager::addAtlasTexture(const texture* const texture)
    {
        auto textureAddress = phi::textureAddress();
        textureContainer* container;
        sizeui containerSize = sizeui(texture->w, texture->h, _maxPages);
        auto layout = texture->layout;

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
                added = container->add(texture, textureAddress);
            }

            if (added)
            {
                _textures[texture] = textureAddress;
                return textureAddress;
            }
        }

        container = createContainer(containerSize, layout);
        container->add(texture, textureAddress);
        _textures[texture] = textureAddress;

        return textureAddress;
    }

    void texturesManager::remove(texture* texture)
    {
        auto address = getTextureAddress(texture);

        auto container = findContainer(address.containerId);

        container->remove(texture);

        if (container->isEmpty())
        {
            if (_isBindless)
            {
                auto handle = container->getHandle();
                phi::removeIfContains(handles, handle);
            }
            else
            {
                auto unit = container->getUnit();
                //TODO: ver se nao eh melhor mandar todas as units disponiveis e nao tratar isso... apenas mandar todas
                //ou entender o pq ao se mudar a ordem o opengl se perde nas units
                phi::removeIfContains(units, unit);
            }

            container->release();
            removeContainer(container);
            safeDelete(container);
        }
    }

    textureAddress texturesManager::add(const texture* const texture)
    {   
        auto layout = texture->layout;
        if (texture->generateMipmaps)
            layout.levels = static_cast<GLsizei>(getMaxLevels(texture->w, texture->h));

        auto textureAddress = phi::textureAddress();
        textureContainer* container;
        sizeui size = sizeui(texture->w, texture->h, _maxPages);

        auto it = _containers.find(layout);
        if (it != _containers.end())
        {
            auto containers = (*it).second;
            uint i = 0;
            bool added = false;
            auto containersCount = containers.size();
            sizeui containerSize = sizeui(0);

            while (!added && i < containersCount)
            {
                container = containers[i++];
                containerSize = container->getSize();
                
                if (containerSize.w != texture->w || containerSize.h != texture->h)
                    continue;

                added = container->add(texture, textureAddress);
            }

            if (added)
            {
                _textures[texture] = textureAddress;
                return textureAddress;
            }
        }

        container = createContainer(size, layout);
        container->add(texture, textureAddress);
        _textures[texture] = textureAddress;

        return textureAddress;
    }

    textureAddress texturesManager::getTextureAddress(const texture* const texture)
    {
        if (!_initialized)
            throw invalidInitializationException("texturesManager not initialized.");

        if (contains(texture))
            return _textures[texture];

        throw keyNotFoundException("texture not found.");
    }

    texture* texturesManager::getTextureFromImage(phi::image* image)
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
            layout.levels = 0;
            layout.dataFormat = textureLayout::translateDataFormat(image->dataFormat);
            layout.dataType = textureLayout::translateDataType(image->dataType);
            layout.internalFormat = GL_RGBA8;
            layout.wrapMode = GL_REPEAT;
            layout.minFilter = GL_LINEAR_MIPMAP_LINEAR;
            layout.magFilter = GL_LINEAR;

            texture = new phi::texture(
                image,
                layout,
                true,
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

        if (_isBindless)
            handles.push_back(container->getHandle());
        else
            units.push_back(container->getUnit());

        return container;
    }

    textureContainer* texturesManager::findContainer(const GLuint containerId)
    {
        for (auto& pair : _containers)
        {
            auto containers = pair.second;

            auto it = std::find_if(
                containers.begin(),
                containers.end(),
                [=](textureContainer* container)
                { 
                    return container->getId() == containerId;
                }
            );

            if (it != containers.end())
                return *it;
        }

        throw argumentException(containerId + " containerId not found.");
    }

    void texturesManager::removeContainer(textureContainer* container)
    {
        phi::removeIfContains(_containers[container->getLayout()], container);
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