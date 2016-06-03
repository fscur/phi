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
            sizeui size,
            textureLayout layout,
            size_t maxTextures);

        ~bindlessTextureContainer();

        void onCreate() override;

        void onLoadData(
            float page,
            const void* const data) override;

        void onLoadSubData(
            const rectangle<GLint>& rect,
            float page,
            const void* const data) override;
    };
}
