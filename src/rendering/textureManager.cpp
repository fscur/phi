#include "textureManager.h"
#include <algorithm>

namespace phi
{
    textureAddress textureManager::add(texture* texture)
    {
        auto biggestTextureSize = (float)glm::max(texture->w, texture->h);
        auto maxLevels = (GLsizei)glm::floor(glm::log2(biggestTextureSize)) + 1.0f;

        auto layout = textureContainerLayout();
        layout.w = texture->w;
        layout.h = texture->h;
        layout.levels = maxLevels;
        layout.internalFormat = texture->internalFormat;

        auto key = std::make_tuple(layout.w, layout.h, layout.levels, layout.internalFormat);

        phi::textureAddress textureAddress;

        auto it = _containers.find(key);
        if (it == _containers.end())
        {
            auto container = new textureContainer(layout, _maxContainerItems, ++_currentTextureUnit, _bindless, _sparse);
            container->add(texture, textureAddress);
            _containers[key].push_back(container);

            units.push_back(_currentTextureUnit);
        }
        else
        {
            auto containers = _containers[key];
            uint i = 0;
            while (!containers[i++]->add(texture, textureAddress)); //TODO: criar novo container quando nao puder add
        }

        return textureAddress;
    }
}