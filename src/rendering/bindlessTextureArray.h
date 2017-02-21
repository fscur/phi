#pragma once
#include <phi.h>
#include "textureArray.h"

namespace phi
{
    class bindlessTextureArray :
        public textureArray
    {
    public:
        bindlessTextureArray(
            sizeui size,
            textureLayout layout);

        ~bindlessTextureArray();

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
