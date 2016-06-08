#include <precompiled.h>
#include "meshRendererDescriptor.h"

#include <core\node.h>
#include <core\notImplementedException.h>

namespace phi
{
    meshRendererDescriptor::meshRendererDescriptor(gl* gl) :
        _gl(gl),
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

        auto albedoTexture = texturesManager::getTextureFromImage(material->albedoImage, _gl->defaultAlbedoImage);
        auto normalTexture = texturesManager::getTextureFromImage(material->normalImage, _gl->defaultNormalImage);
        auto specularTexture = texturesManager::getTextureFromImage(material->specularImage, _gl->defaultSpecularImage);
        auto emissiveTexture = texturesManager::getTextureFromImage(material->emissiveImage, _gl->defaultEmissiveImage);

        auto albedoTextureAddress = _gl->texturesManager->get(albedoTexture);
        auto normalTextureAddress = _gl->texturesManager->get(normalTexture);
        auto specularTextureAddress = _gl->texturesManager->get(specularTexture);
        auto emissiveTextureAddress = _gl->texturesManager->get(emissiveTexture);

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
        if (mesh->material == nullptr)
            mesh->material = _gl->defaultMaterial;

        uploadMaterialIfNew(mesh->material);

        renderInstance instance;
        instance.mesh = mesh;
        instance.modelMatrix = mesh->getNode()->getTransform()->getModelMatrix();
        instance.materialId = _materialsIndices[instance.mesh->material];

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
}