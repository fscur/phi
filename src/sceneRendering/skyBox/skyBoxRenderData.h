#pragma once
#include <core/skyBox.h>
#include <rendering/texturesManager.h>
#include <rendering/cubeMap.h>

namespace phi
{
    struct skyBoxRenderData
    {
        int cubeMapUnit;
        int ipad0;
        int ipad1;
        int ipad2;
        float cubeMapPage;
        float fpad0;
        float fpad1;
        float fpad2;

        static skyBoxRenderData from(const skyBox* skyBox)
        {
            auto data = skyBoxRenderData();

            auto skyBoxImages = skyBox->getImages();
            vector<const image*> images =
            {
                skyBoxImages.PositiveX,
                skyBoxImages.NegativeX,
                skyBoxImages.PositiveY,
                skyBoxImages.NegativeY,
                skyBoxImages.PositiveZ,
                skyBoxImages.NegativeZ
            };

            auto layout = textureLayout();
            layout.dataFormat = textureLayout::translateDataFormat(images[0]->dataFormat);
            layout.dataType = textureLayout::translateDataType(images[0]->dataType);
            layout.internalFormat = GL_RGBA8;
            layout.wrapMode = GL_CLAMP_TO_EDGE;
            layout.minFilter = GL_LINEAR_MIPMAP_LINEAR;
            layout.magFilter = GL_LINEAR;
            layout.levels = static_cast<GLsizei>(texturesManager::getMaxLevels(images[0]->w, images[0]->h));

            auto cubeMap = new phi::cubeMap(images, layout);

            textureAddress address;

            if (!texturesManager::contains(cubeMap))
                address = texturesManager::addCubeMap(cubeMap);
            else
                address = texturesManager::getCubeMapAddress(cubeMap);

            data.cubeMapUnit = address.index;
            data.cubeMapPage = address.page;

            return data;
        }
    };
}
