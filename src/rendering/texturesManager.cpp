#include <precompiled.h>
#include "texturesManager.h"

namespace phi
{
    texturesManager::texturesManager(
        bool bindless = false,
        bool sparse = false) :
        _bindless(bindless),
        _sparse(sparse),
        _currentTextureUnit(-1),
        _maxContainerSize(MAX_CONTAINER_ITEMS)
    {
        //TODO: calcular quanto de memoria tem disponivel na GPU
        //TODO: verificar quando de memoria nosso gbuffer + shadow maps usam e ver quanto sobra pra texturas
        //TODO: controlar a memoria da gpu usada

        if (_sparse)
        {
            GLint maxContainerSize;
            glGetIntegerv(GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS_ARB, &maxContainerSize);
            phi::glError::check();
            _maxContainerSize = static_cast<size_t>(maxContainerSize);
        }

        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, &_maxTextureUnits);
    }

    texturesManager::~texturesManager()
    {
        for (auto &pair : _containers)
        {
            for (auto container : pair.second)
                safeDelete(container);
        }
    }

    textureAddress texturesManager::add(const texture* const texture)
    {
        GLsizei maxLevels = 1;

        if (texture->generateMipmaps)
        {
            auto biggestTextureSize = (float)glm::max(texture->w, texture->h);
            maxLevels = static_cast<GLsizei>(glm::floor(glm::log2(biggestTextureSize)) + 1.0f);
        }

        auto layout = textureContainerLayout();
        layout.w = texture->w;
        layout.h = texture->h;
        layout.levels = maxLevels;
        layout.internalFormat = texture->internalFormat;
        layout.wrapMode = texture->wrapMode;
        layout.minFilter = texture->minFilter;
        layout.magFilter = texture->magFilter;

        auto key = std::make_tuple(
            layout.w,
            layout.h,
            layout.levels,
            layout.internalFormat,
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
                return textureAddress;
            }
        }

        auto container = new textureContainer(layout, _maxContainerSize, ++_currentTextureUnit, _bindless, _sparse);
        container->add(texture, textureAddress);
        _containers[key].push_back(container);
        handles.push_back(container->handle);
        units.push_back(_currentTextureUnit);

        return textureAddress;
    }

    textureAddress texturesManager::get(const texture* const texture)
    {
        return _textures[texture];
    }

    bool texturesManager::contains(const texture* const texture)
    {
        return _textures.find(texture) != _textures.end();
    }

    void texturesManager::reserveContainer(textureContainerLayout layout, size_t size)
    {
        auto key = std::make_tuple(
            layout.w,
            layout.h,
            layout.levels,
            layout.internalFormat,
            layout.wrapMode,
            layout.minFilter,
            layout.magFilter);

        auto container = new textureContainer(layout, std::min(_maxContainerSize, size), ++_currentTextureUnit, _bindless, _sparse);
        _containers[key].push_back(container);
        handles.push_back(container->handle);
        units.push_back(_currentTextureUnit);
    }
}