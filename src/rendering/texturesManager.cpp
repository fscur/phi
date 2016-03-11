#include <precompiled.h>
#include "texturesManager.h"

namespace phi
{
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

    textureAddress texturesManager::add(texture* texture)
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
                return textureAddress;
        }

        auto container = new textureContainer(layout, _maxContainerSize, ++_currentTextureUnit, _bindless, _sparse);
        container->add(texture, textureAddress);
        _containers[key].push_back(container);
        handles.push_back(container->handle);
        units.push_back(_currentTextureUnit);

        return textureAddress;
    }
}