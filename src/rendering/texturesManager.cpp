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
        _maxContainerSize(MAX_CONTAINER_ITEMS)
    {
        assert(!_initialized);
        _initialized = true;

        //TODO: calcular quanto de memoria tem disponivel na GPU
        //TODO: verificar quando de memoria nosso gbuffer + shadow maps usam e ver quanto sobra pra texturas
        //TODO: controlar a memoria da gpu usada

        if (_sparse)
        {
            GLint maxContainerSize;
            glGetIntegerv(GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS, &maxContainerSize);
            phi::glError::check();
            _maxContainerSize = static_cast<size_t>(maxContainerSize);
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
        GLsizei maxLevels = 1;

        if (texture->generateMipmaps)
            maxLevels = static_cast<GLsizei>(getMaxLevels(texture->w, texture->h));

        auto layout = textureContainerLayout();
        layout.w = texture->w;
        layout.h = texture->h;
        layout.levels = maxLevels;
        layout.internalFormat = texture->internalFormat;
        layout.dataFormat = texture->dataFormat;
        layout.dataType = texture->dataType;
        layout.wrapMode = texture->wrapMode;
        layout.minFilter = texture->minFilter;
        layout.magFilter = texture->magFilter;

        auto key = std::make_tuple(
            layout.w,
            layout.h,
            layout.levels,
            layout.internalFormat,
            layout.dataFormat,
            layout.dataType,
            layout.wrapMode,
            layout.minFilter,
            layout.magFilter);

        auto textureAddress = phi::textureAddress();

        auto it = _containers.find(key);
        if (it != _containers.end())
        {
            auto containers = _containers[key];
            uint i = 0;
            bool added = false;
            auto containersCount = containers.size();

            while (!added && i < containersCount)
                added = containers[i++]->add(texture, textureAddress);

            if (added)
            {
                _textures[texture] = textureAddress;
                return;
            }
        }

        textureContainer* container;

        if (_sparse && _bindless)
            container = new sparseBindlessTextureContainer(layout, _maxContainerSize);
        else if (_sparse)
            container = new sparseTextureContainer(layout, _maxContainerSize);
        else if (_bindless)
            container = new bindlessTextureContainer(layout, _maxContainerSize);
        else
            container = new textureContainer(layout, _maxContainerSize);

        container->add(texture, textureAddress);
        _containers[key].push_back(container);
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
            texture = new phi::texture(
                materialImage,
                GL_TEXTURE_2D,
                GL_RGBA8,
                GL_REPEAT,
                GL_LINEAR_MIPMAP_LINEAR,
                GL_LINEAR,
                true);

            _imageTextures[materialImage] = texture;
        }

        return texture;
    }

    bool texturesManager::contains(const texture* const texture)
    {
        return phi::contains(_textures, texture);
    }

    textureContainer* texturesManager::reserveContainer(textureContainerLayout layout, size_t size)
    {
        auto key = std::make_tuple(
            layout.w,
            layout.h,
            layout.levels,
            layout.internalFormat,
            layout.dataFormat,
            layout.dataType,
            layout.wrapMode,
            layout.minFilter,
            layout.magFilter);

        auto maxTextures = std::min(_maxContainerSize, size);

        textureContainer* container;

        if (_sparse && _bindless)
            container = new sparseBindlessTextureContainer(layout, maxTextures);
        else if (_sparse)
            container = new sparseTextureContainer(layout, maxTextures);
        else if (_bindless)
            container = new bindlessTextureContainer(layout, maxTextures);
        else
            container = new textureContainer(layout, maxTextures);

        _containers[key].push_back(container);
        handles.push_back(container->getHandle());
        units.push_back(container->getUnit());
        return container;
    }
}