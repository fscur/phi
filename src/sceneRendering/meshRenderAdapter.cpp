#include <precompiled.h>
#include "meshRenderAdapter.h"

#include <core\node.h>

#include <rendering\texturesManager.h>

namespace phi
{
    meshRenderAdapter::meshRenderAdapter() :
        onBatchAdded(new eventHandler<batch*>())
    {
        createBuffers();
    }

    meshRenderAdapter::~meshRenderAdapter()
    {
        safeDelete(_materialRenderDataBuffer);

        for (auto batch : _batches)
            safeDelete(batch);

        _onDelete();
        safeDelete(onBatchAdded);
    }

    void meshRenderAdapter::createBuffers()
    {
        _materialRenderDataBuffer = new buffer("MaterialRenderDataBuffer", bufferTarget::shader);

        _materialRenderDataBuffer->storage(
            sizeof(materialRenderData) * MAX_MATERIALS_COUNT,
            nullptr,
            bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void meshRenderAdapter::uploadMaterialIfNew(material* material)
    {
        static uint currentId = 0;

        if (phi::contains(_materialsIndices, material))
            return;

        _materialsIndices[material] = currentId++;

        auto albedoTexture = texturesManager::getTextureFromImage(material->albedoImage);
        auto normalTexture = texturesManager::getTextureFromImage(material->normalImage);
        auto specularTexture = texturesManager::getTextureFromImage(material->specularImage);
        auto emissiveTexture = texturesManager::getTextureFromImage(material->emissiveImage);

        auto getTextureAdress = [](const texture* texture)
        {
            if (!texturesManager::contains(texture))
                return texturesManager::addTexture(texture);
            else
                return texturesManager::getTextureAddress(texture);
        };

        auto albedoTextureAddress = getTextureAdress(albedoTexture);
        auto normalTextureAddress = getTextureAdress(normalTexture);
        auto specularTextureAddress = getTextureAdress(specularTexture);
        auto emissiveTextureAddress = getTextureAdress(emissiveTexture);

        auto materialGpuData = phi::materialRenderData(
            albedoTextureAddress,
            normalTextureAddress,
            specularTextureAddress,
            emissiveTextureAddress,
            material);

        auto offset = _materialsIndices[material] * sizeof(phi::materialRenderData);
        _materialRenderDataBuffer->subData(offset, sizeof(phi::materialRenderData), &materialGpuData);
    }

    void meshRenderAdapter::addToBatch(meshInstance* instance, batch* batch)
    {
        batch->add(instance);
        _meshesBatches[instance->mesh] = batch;
    }

    void meshRenderAdapter::addNewBatch(meshInstance* instance)
    {
        auto batch = new phi::batch();
        addToBatch(instance, batch);

        _batches.push_back(batch);
        onBatchAdded->raise(batch);
    }

    void meshRenderAdapter::addToBatch(meshInstance* instance)
    {
        //TODO: make sure geometries that have already been added are added to the same batch
        for (auto& batch : _batches)
        {
            if (batch->canAdd(instance))
            {
                addToBatch(instance, batch);
                return;
            }
        }

        addNewBatch(instance);
    }

    meshInstance* meshRenderAdapter::createMeshInstance(mesh* mesh)
    {
        auto materialId = _materialsIndices[mesh->getMaterial()];
        auto instance = new meshInstance(mesh, materialId, mesh->getModelMatrix());
        _meshesInstances[mesh] = instance;

        return instance;
    }

    void meshRenderAdapter::add(mesh* mesh)
    {
        uploadMaterialIfNew(mesh->getMaterial());

        auto instance = createMeshInstance(mesh);
        addToBatch(instance);
    }

    void meshRenderAdapter::remove(mesh* mesh)
    {
        auto instance = _meshesInstances[mesh];
        auto batch = _meshesBatches[mesh];

        batch->remove(instance);

        _meshesInstances.erase(mesh);
        _meshesBatches.erase(mesh);

        if (batch->isEmpty())
        {
            phi::removeIfContains(_batches, batch);
            safeDelete(batch);
        }

        //TODO: remove material from gpu if this is the last mesh using it
    }

    void meshRenderAdapter::update(mesh* mesh)
    {
        auto instance = _meshesInstances[mesh];
        _meshesBatches[mesh]->update(instance);
    }

    void meshRenderAdapter::updateSelection(mesh* mesh, bool isSelected)
    {
        auto instance = _meshesInstances[mesh];
        _meshesBatches[mesh]->updateSelectionBuffer(instance, isSelected);
    }

    void meshRenderAdapter::updateTransform(mesh* mesh, const mat4& modelMatrix)
    {
        auto instance = _meshesInstances[mesh];
        instance->modelMatrix = modelMatrix;
        _meshesBatches[mesh]->updateTransformBuffer(instance);
    }
}