#include <precompiled.h>

#include <core/modelNode.h>
#include <core/mesh.h>
#include <core/exception.h>
#include <core/boxCollider.h>

#include <animation/animator.h>

#include <common/mouseInteractionComponent.h>

#include "modelDataService.h"

namespace phi
{
    modelDataService::modelDataService(
        iGeometryRepository* geometryRepository,
        iMaterialRepository* materialRepository,
        iTextureRepository* textureRepository,
        iModelRepository* modelRepository) :
        _geometryRepository(geometryRepository),
        _materialRepository(materialRepository),
        _imageRepository(textureRepository),
        _modelRepository(modelRepository)
    {
    }

    modelDataService::~modelDataService()
    {
    }

    model* modelDataService::getModelByIndex(int index)
    {
        //implement cache
        auto model = _modelRepository->getModelByIndex(index);

        auto node = assembleNode(model.node);
        addDefaultComponents(node);

        return new phi::model(model.guid, node);
    }

    model* modelDataService::getModelById(const phi::guid& id)
    {
        //implement cache
        auto model = _modelRepository->getModelById(id);

        auto node = assembleNode(model.node);
        addDefaultComponents(node);

        return new phi::model(model.guid, node);
    }

    void modelDataService::addDefaultComponents(node* node)
    {
        node->addComponent(new phi::mouseInteractionComponent());
    }

    node* modelDataService::assembleNode(nodeDTO nodeDTO)
    {
        auto node = new phi::node();
        for (auto& component : nodeDTO.components)
        {
            node->addComponent(assembleComponent(component));
        }

        for (auto& child : nodeDTO.children)
        {
            node->addChild(assembleNode(child));
        }

        return node;
    }

    component* modelDataService::assembleComponent(componentDTO componentDTO)
    {
        switch (componentDTO.type)
        {
        case component::componentType::MODEL_NODE:
            return new modelNode();
        case component::componentType::MESH:
            return new mesh(
                _geometryRepository->getById(componentDTO.geometryResourceGuid),
                _materialRepository->getById(componentDTO.materialResourceGuid)
            );
        default:
            break;
        }

        throw exception("There isn't a implemented parse for a component of type " + std::to_string(componentDTO.type));
    }
}