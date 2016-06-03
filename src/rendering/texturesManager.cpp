#include <precompiled.h>
#include "texturesManager.h"
#include "bindlessTextureContainer.h"
#include "sparseTextureContainer.h"
#include "sparseBindlessTextureContainer.h"
#include "textureUnits.h"

namespace phi
{
    bool texturesManager::_initialized = false;
    map<image*, texture*> texturesManager::_imageTextures = map<image*, texture*>();

    texturesManager::texturesManager(
        bool bindless = false,
        bool sparse = false) :
        _bindless(bindless),
        _sparse(sparse),
        _maxPages(MAX_CONTAINER_PAGES)
    {
        assert(!_initialized);
        _initialized = true;

        //TODO: calcular quanto de memoria tem disponivel na GPU
        //TODO: verificar quando de memoria nosso gbuffer + shadow maps usam e ver quanto sobra pra texturas
        //TODO: controlar a memoria da gpu usada

        if (_sparse)
        {
            GLint maxPages;
            glGetIntegerv(GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS, &maxPages);
            phi::glError::check();
            _maxPages = static_cast<size_t>(maxPages);
        }

        textureUnits::init();
    }

    texturesManager::~texturesManager()
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

        auto it = _containers.find(layout);
        if (it != _containers.end())
        {
            auto containers = (*it).second;
            uint i = 0;
            bool added = false;
            auto containersCount = containers.size();

            while (!added && i < containersCount)
            {
                auto container = containers[i++];
                auto containerSize = container->getSize();
                
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

        textureContainer* container;

        auto size = sizeui(texture->w, texture->h);

        if (_sparse && _bindless)
            container = new sparseBindlessTextureContainer(size, layout, _maxPages);
        else if (_sparse)
            container = new sparseTextureContainer(size, layout, _maxPages);
        else if (_bindless)
            container = new bindlessTextureContainer(size, layout, _maxPages);
        else
            container = new textureContainer(size, layout, _maxPages);

        container->add(texture, textureAddress);
        _containers[layout].push_back(container);
        handles.push_back(container->getHandle());
        units.push_back(container->getUnit());

        _textures[texture] = textureAddress;
    }

    textureAddress texturesManager::get(const texture* const texture)
    {
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

    textureContainer* texturesManager::reserveContainer(sizeui size, textureLayout layout, size_t pages)
    {
        auto maxPages = std::min(_maxPages, pages);

        textureContainer* container;

        if (_sparse && _bindless)
            container = new sparseBindlessTextureContainer(size, layout, maxPages);
        else if (_sparse)
            container = new sparseTextureContainer(size, layout, maxPages);
        else if (_bindless)
            container = new bindlessTextureContainer(size, layout, maxPages);
        else
            container = new textureContainer(size, layout, maxPages);

        _containers[layout].push_back(container);
        handles.push_back(container->getHandle());
        units.push_back(container->getUnit());
        return container;
    }
}