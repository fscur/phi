#pragma once
#include <phi.h>
#include "textureContainer.h"

namespace phi
{
    class sparseBindlessTextureContainer :
        public textureContainer
    {
    public:
        sparseBindlessTextureContainer(
            textureContainerLayout layout,
            size_t maxTextures);

        ~sparseBindlessTextureContainer();

        void onCreate() override;
        void onLoadTexture(const texture* const texture) override;
        void onSubData(
            const float& page,
            const rectangle<GLint>& rect,
            const void* const data) override;
    };
}