#pragma once
#include <phi.h>
#include "textureArray.h"

namespace phi
{
    class sparseTextureArray :
        public textureArray
    {
    public:
        sparseTextureArray(
            sizeui size,
            textureLayout layout);

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