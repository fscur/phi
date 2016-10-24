#pragma once
#include <phi.h>

#include "dataApi.h"
#include "abstractions/iGeometryRepository.h"
#include "abstractions/iMaterialRepository.h"
#include "abstractions/iImageRepository.h"
#include "abstractions/iModelRepository.h"
#include "abstractions/iModelDataService.h"

namespace phi
{
    class modelDataService : public iModelDataService
    {
    private:
        iGeometryRepository* _geometryRepository;
        iMaterialRepository* _materialRepository;
        iImageRepository* _imageRepository;
        iModelRepository* _modelRepository;

    public:
        DATA_API modelDataService(iGeometryRepository* geometryRepository,
                         iMaterialRepository* materialRepository,
                         iImageRepository* imageRepository,
                         iModelRepository* modelRepository);
        DATA_API ~modelDataService();

        DATA_API virtual model* getModelByIndex(int index) override;
    };
}