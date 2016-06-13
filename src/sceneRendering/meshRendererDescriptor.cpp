#include <precompiled.h>
#include <core\node.h>
#include <core\notImplementedException.h>
#include <rendering\texturesManager.h>

#include "meshRendererDescriptor.h"

namespace phi
{
    meshRendererDescriptor::meshRendererDescriptor() :
        onBatchAdded(new eventHandler<batch*>())
    {
        createBuffers();
    }

    meshRendererDescriptor::~meshRendererDescriptor()
    {
        safeDelete(_materialRenderDataBuffer);

        for (auto batch : _batches)
            safeDelete(batch);
    }

    void meshRendererDescriptor::createBuffers()
    {
        _materialRenderDataBuffer = new buffer("MaterialRenderDataBuffer", bufferTarget::shader);

        _materialRenderDataBuffer->storage(
            sizeof(materialRenderData) * MAX_MATERIALS_COUNT,
            nullptr,
            bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void meshRendererDescriptor::uploadMaterialIfNew(material* material)
    {
        static uint currentId = 0;

        if (phi::contains(_materialsIndices, material))
            return;

        _materialsIndices[material] = currentId++;

        auto albedoTexture = texturesManager::getTextureFromImage(material->albedoImage);
        auto normalTexture = texturesManager::getTextureFromImage(material->normalImage);
        auto specularTexture = texturesManager::getTextureFromImage(material->specularImage);
        auto emissiveTexture = texturesManager::getTextureFromImage(material->emissiveImage);

        auto albedoTextureAddress = texturesManager::get(albedoTexture);
        auto normalTextureAddress = texturesManager::get(normalTexture);
        auto specularTextureAddress = texturesManager::get(specularTexture);
        auto emissiveTextureAddress = texturesManager::get(emissiveTexture);

        auto materialGpuData = phi::materialRenderData(
            albedoTextureAddress,
            normalTextureAddress,
            specularTextureAddress,
            emissiveTextureAddress,
            material);

        auto offset = _materialsIndices[material] * sizeof(phi::materialRenderData);
        _materialRenderDataBuffer->subData(offset, sizeof(phi::materialRenderData), &materialGpuData);
    }

    void meshRendererDescriptor::add(mesh * mesh)
    {
        uploadMaterialIfNew(mesh->getMaterial());

        renderInstance instance;
        instance.mesh = mesh;
        instance.modelMatrix = mesh->getNode()->getTransform()->getModelMatrix();
        instance.materialId = _materialsIndices[instance.mesh->getMaterial()];

        auto i = 0u;
        auto added = false;
        auto batchesCount = _batches.size();

        batch* batch = nullptr;

        while (!added && i < batchesCount)
        {
            batch = _batches[i++];
            added = batch->add(instance);
        }

        if (!added)
        {
            batch = new phi::batch();
            _batches.push_back(batch);
            batch->add(instance);

            onBatchAdded->raise(batch); //TODO: AAAAAAAAAAAAAAAAAAAAAA SALVE OS TOKEN SEU BOSTA
        }

        _meshesBatches[instance.mesh] = batch;
    }

    void meshRendererDescriptor::remove(mesh * mesh)
    {
        throw notImplementedException();
    }

    void meshRendererDescriptor::update(mesh * mesh)
    {
        throw notImplementedException();
    }

    void meshRendererDescriptor::updateSelectionBuffer(mesh* mesh, bool isSelected)
    {
        _meshesBatches[mesh]->updateSelectionBuffer(mesh, isSelected);
    }
}