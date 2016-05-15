#include <precompiled.h>
#include "pipeline.h"

#include "materialGpuData.h"

namespace phi
{
    pipeline::pipeline(gl* gl, float w, float h) :
        _gl(gl),
        _renderer(new renderer(gl, w, h))
    {
        createFrameUniformBlockBuffer();
        createMaterialsBuffer();
    }

    pipeline::~pipeline()
    {
        safeDelete(_materialsBuffer);
        safeDelete(_frameUniformBlockBuffer);
        safeDelete(_renderer);

        for (auto batch : _batches)
            safeDelete(batch);
    }

    void pipeline::createFrameUniformBlockBuffer()
    {
        _frameUniformBlockBuffer = new buffer(bufferTarget::uniform);

        _frameUniformBlockBuffer->storage(
            sizeof(phi::frameUniformBlock),
            nullptr,
            bufferStorageUsage::dynamic | bufferStorageUsage::write);

    }

    void pipeline::createMaterialsBuffer()
    {
        auto materialsBufferSize = MAX_MATERIALS_COUNT * sizeof(materialGpuData);
        _materialsBuffer = new buffer(bufferTarget::shader);
        _materialsBuffer->storage(materialsBufferSize, nullptr, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void pipeline::uploadMaterial(material * material)
    {
        static uint currentId = 0;
        if (!phi::contains(_materialsCache, material))
            _materialsCache[material] = currentId++;

        auto albedoTextureAddress = _gl->texturesManager->add(material->albedoTexture);
        auto normalTextureAddress = _gl->texturesManager->add(material->normalTexture);
        auto specularTextureAddress = _gl->texturesManager->add(material->specularTexture);
        auto emissiveTextureAddress = _gl->texturesManager->add(material->emissiveTexture);

        auto materialGpuData = phi::materialGpuData(
            albedoTextureAddress.unit,
            normalTextureAddress.unit,
            specularTextureAddress.unit,
            emissiveTextureAddress.unit,
            albedoTextureAddress.page,
            normalTextureAddress.page,
            specularTextureAddress.page,
            emissiveTextureAddress.page,
            material->albedoColor,
            material->specularColor,
            material->emissiveColor,
            material->shininess,
            material->reflectivity,
            material->emission,
            material->opacity);

        auto offset = _materialsCache[material] * sizeof(phi::materialGpuData);

        _materialsBuffer->subData(offset, sizeof(phi::materialGpuData), &materialGpuData);
        _materialsBuffer->bindBufferBase(1);
    }

    void pipeline::add(renderInstance& instance)
    {
        if (!phi::contains(_materialsCache, instance.mesh->material))
            uploadMaterial(instance.mesh->material);
        instance.materialId = _materialsCache[instance.mesh->material];

        auto i = 0u;
        auto added = false;
        auto batchesCount = _batches.size();

        phi::batch* batch = nullptr;

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
        }

        _meshesBatches[instance.mesh] = batch;
    }

    void pipeline::remove(const renderInstance& instance)
    {
    }

    void pipeline::update(const renderInstance& instance)
    {
    }

    void pipeline::update(const frameUniformBlock& frameUniformBlock)
    {
        _frameUniformBlockBuffer->subData(0, sizeof(phi::frameUniformBlock), &frameUniformBlock);
        _frameUniformBlockBuffer->bindBufferBase(0);

        _renderer->update();
    }

    void pipeline::render()
    {
        _renderer->render(_batches);
    }
}