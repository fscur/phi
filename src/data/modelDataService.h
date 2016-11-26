#pragma once
#include <phi.h>

#include "dataApi.h"
#include "abstractions/iGeometryRepository.h"
#include "abstractions/iMaterialRepository.h"
#include "abstractions/iTextureRepository.h"
#include "abstractions/iModelRepository.h"
#include "abstractions/iModelDataService.h"

namespace phi
{
    class modelDataService : public iModelDataService
    {
    public:
        DATA_API modelDataService(iGeometryRepository* geometryRepository,
                         iMaterialRepository* materialRepository,
                         iTextureRepository* textureRepository,
                         iModelRepository* modelRepository);
        DATA_API ~modelDataService();

        DATA_API virtual model* getModelByIndex(int index) override;


    private:
        node * assembleNode(nodeDTO node);
        component * assembleComponent(componentDTO componentDTO);
        void addDefaultComponents(node * node);

    private:
        iGeometryRepository* _geometryRepository;
        iMaterialRepository* _materialRepository;
        iTextureRepository* _imageRepository;
        iModelRepository* _modelRepository;
    };
}