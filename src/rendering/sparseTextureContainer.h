#pragma once
#include <phi.h>
#include "textureContainer.h"

namespace phi
{
    class sparseTextureContainer :
        public textureContainer
    {
    public:
        sparseTextureContainer(
            sizeui size,
            textureLayout layout,
            size_t maxTextures);

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