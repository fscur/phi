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
        safeDelete(_frameUniformBlockBuffer);
        safeDelete(_materialsBuffer);
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
        _materialsBuffer = new buffer(bufferTarget::shader);
        
        _materialsBuffer->storage(
            sizeof(materialGpuData) * MAX_MATERIALS_COUNT,
            nullptr,
            bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void pipeline::uploadMaterialIfNew(material* material)
    {
        static uint currentId = 0;

        if (phi::contains(_materialsIndices, material))
            return;

        _materialsIndices[material] = currentId++;

        auto albedoTextureAddress = _gl->texturesManager->add(material->albedoTexture);
        auto normalTextureAddress = _gl->texturesManager->add(material->normalTexture);
        auto specularTextureAddress = _gl->texturesManager->add(material->specularTexture);
        auto emissiveTextureAddress = _gl->texturesManager->add(material->emissiveTexture);

        auto materialGpuData = phi::materialGpuData(
            albedoTextureAddress,
            normalTextureAddress,
            specularTextureAddress,
            emissiveTextureAddress,
            material);

        auto offset = _materialsIndices[material] * sizeof(phi::materialGpuData);
        _materialsBuffer->subData(offset, sizeof(phi::materialGpuData), &materialGpuData);
        _materialsBuffer->bindBufferBase(1);
    }

    void pipeline::add(renderInstance& instance)
    {
        uploadMaterialIfNew(instance.mesh->material);
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