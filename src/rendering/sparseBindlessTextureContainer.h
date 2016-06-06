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
            sizeui size,
            textureLayout layout);

        ~sparseBindlessTextureContainer();

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