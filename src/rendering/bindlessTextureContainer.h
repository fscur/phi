#pragma once
#include <phi.h>
#include "textureContainer.h"

namespace phi
{
    class bindlessTextureContainer :
        public textureContainer
    {
    public:
        bindlessTextureContainer(
            textureContainerLayout layout,
            size_t maxTextures,
            GLint unit);

        ~bindlessTextureContainer();

        void onCreate() override;
        void onLoadTexture(const texture* const texture) override;
        void onSubData(
            const float& page,
            const rectangle<GLint>& rect,
            const void* const data) override;
    };
}
