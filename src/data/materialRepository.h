#pragma once
#include <phi.h>

#include <core/material.h>

#include "dataApi.h"
#include "abstractions/iMaterialRepository.h"
#include "abstractions/iTextureRepository.h"
#include "model/index.h"

namespace phi
{
    class materialRepository : public iMaterialRepository
    {
    public:
        DATA_API materialRepository(string libraryPath, iTextureRepository* textureRepository);
        DATA_API ~materialRepository();

        DATA_API virtual material* getById(const guid& guid);

    private:
        const string MATERIALS_INDEX_FILE_NAME = "materials.index";
        string _libraryPath;
        iTextureRepository* _imageRepository;
        index _index;
        unordered_map<guid, material*, guidComparer> _cache;
    };
}