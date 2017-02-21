#pragma once
#include <phi.h>

#include <core/guid.h>

#include "dataApi.h"
#include "abstractions/iTextureRepository.h"
#include "model/index.h"

namespace phi
{
    class textureRepository : public iTextureRepository
    {
    public:
        DATA_API textureRepository(string libraryPath);
        DATA_API ~textureRepository();

        virtual image* getById(guid guid) override;

    private:
        image * loadImage(string filePath, guid guid);

    private:
        const string TEXTURES_INDEX_FILE_NAME = "textures.index";
        string _libraryPath;
        index _index;
    };
}