#include <precompiled.h>

#include "modelDataService.h"

namespace phi
{
    modelDataService::modelDataService(
        iGeometryRepository* geometryRepository,
        iMaterialRepository* materialRepository,
        iImageRepository* imageRepository,
        iModelRepository* modelRepository) : 
        _geometryRepository(geometryRepository),
        _materialRepository(materialRepository),
        _imageRepository(imageRepository),
        _modelRepository(modelRepository)
    {
    }

    modelDataService::~modelDataService()
    {
    }

    model* modelDataService::getModelByIndex(int index)
    {
        auto model = _modelRepository->getModelByIndex(index);

        //auto node = readNode(model.node);

        //return new phi::model(model.guid, node);

        return nullptr;
    }
}