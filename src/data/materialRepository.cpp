#include <precompiled.h>

#include <io/path.h>

#include "materialRepository.h"
#include "model/materialDTO.h"
#include "utils/cerealSerializationFunctions.h"

namespace phi
{
    using namespace cereal;

    materialRepository::materialRepository(string libraryPath, iTextureRepository* textureRepository) :
        _libraryPath(libraryPath),
        _imageRepository(textureRepository)
    {
        auto indexPath = path::combine(libraryPath, MATERIALS_INDEX_FILE_NAME);
        _index = index::load(indexPath);
    }

    materialRepository::~materialRepository()
    {
    }

    material* materialRepository::getById(const guid& guid)
    {
        auto entry = _index.getEntryById(guid);
        auto materialPath = path::combine(_libraryPath, entry.path);
        std::ifstream inputFileStream(materialPath);
        JSONInputArchive archive(inputFileStream);

        materialDTO materialDTO;
        materialDTO.serialize(archive);

        auto albedoImage = _imageRepository->getById(materialDTO.albedoTextureGuid);
        auto normalImage = _imageRepository->getById(materialDTO.normalTextureGuid);
        auto specularImage = _imageRepository->getById(materialDTO.specularTextureGuid);
        auto emissiveImage = _imageRepository->getById(materialDTO.emissiveTextureGuid);

        if (!albedoImage)
            albedoImage = image::defaultAlbedoImage;

        if (!normalImage)
            normalImage = image::defaultNormalImage;

        if (!specularImage)
            specularImage = image::defaultSpecularImage;

        if (!emissiveImage)
            emissiveImage = image::defaultEmissiveImage;

        return new material(
            materialDTO.guid,
            albedoImage,
            normalImage,
            specularImage,
            emissiveImage,
            materialDTO.albedoColor.toVec3(),
            materialDTO.specularColor.toVec3(),
            materialDTO.emissiveColor.toVec3(),
            materialDTO.shininess,
            materialDTO.reflectivity,
            materialDTO.emission,
            materialDTO.opacity);
    }
}