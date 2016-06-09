#include <precompiled.h>
#include "pipeline.h"
//
//namespace phi
//{
//    pipeline::pipeline(gl* gl, float w, float h) :
//        _gl(gl)
//    {
//        createFrameUniformBlockBuffer();
//        createMaterialsBuffer();
//    }
//
//    pipeline::~pipeline()
//    {
//        safeDelete(_frameUniformBlockBuffer);
//        safeDelete(_materialRenderDataBuffer);
//
//        for (auto batch : _batches)
//            safeDelete(batch);
//    }
//
//    void pipeline::createFrameUniformBlockBuffer()
//    {
//        _frameUniformBlockBuffer = new buffer("FrameUniformsBufferData", bufferTarget::uniform);
//
//        _frameUniformBlockBuffer->storage(
//            sizeof(phi::frameUniformBlock),
//            nullptr,
//            bufferStorageUsage::dynamic | bufferStorageUsage::write);
//    }
//
//    void pipeline::createMaterialsBuffer()
//    {
//        _materialRenderDataBuffer = new buffer("MaterialsBufferData", bufferTarget::shader);
//
//        _materialRenderDataBuffer->storage(
//            sizeof(materialRenderData) * MAX_MATERIALS_COUNT,
//            nullptr,
//            bufferStorageUsage::dynamic | bufferStorageUsage::write);
//    }
//
//    void pipeline::uploadMaterialIfNew(material* material)
//    {
//        static uint currentId = 0;
//
//        if (phi::contains(_materialsIndices, material))
//            return;
//
//        _materialsIndices[material] = currentId++;
//
//        auto albedoTexture = texturesManager::getTextureFromImage(material->albedoImage, _gl->defaultAlbedoImage);
//        auto normalTexture = texturesManager::getTextureFromImage(material->normalImage, _gl->defaultNormalImage);
//        auto specularTexture = texturesManager::getTextureFromImage(material->specularImage, _gl->defaultSpecularImage);
//        auto emissiveTexture = texturesManager::getTextureFromImage(material->emissiveImage, _gl->defaultEmissiveImage);
//
//        auto albedoTextureAddress = _gl->texturesManager->get(albedoTexture);
//        auto normalTextureAddress = _gl->texturesManager->get(normalTexture);
//        auto specularTextureAddress = _gl->texturesManager->get(specularTexture);
//        auto emissiveTextureAddress = _gl->texturesManager->get(emissiveTexture);
//
//        auto materialGpuData = phi::materialRenderData(
//            albedoTextureAddress,
//            normalTextureAddress,
//            specularTextureAddress,
//            emissiveTextureAddress,
//            material);
//
//        auto offset = _materialsIndices[material] * sizeof(phi::materialRenderData);
//        _materialRenderDataBuffer->subData(offset, sizeof(phi::materialRenderData), &materialGpuData);
//        _materialRenderDataBuffer->bindBufferBase(1);
//    }
//
//    void pipeline::add(mesh* mesh, mat4 modelMatrix)
//    {
//        if (mesh->material == nullptr)
//            mesh->material = _gl->defaultMaterial;
//
//        uploadMaterialIfNew(mesh->material);
//
//        renderInstance instance = { 0 };
//        instance.mesh = mesh;
//        instance.modelMatrix = modelMatrix;
//        instance.materialId = _materialsIndices[instance.mesh->material];
//
//        auto i = 0u;
//        auto added = false;
//        auto batchesCount = _batches.size();
//
//        batch* batch = nullptr;
//
//        while (!added && i < batchesCount)
//        {
//            batch = _batches[i++];
//            added = batch->add(instance);
//        }
//
//        if (!added)
//        {
//            batch = new phi::batch();
//            _batches.push_back(batch);
//            batch->add(instance);
//        }
//
//        _meshesBatches[instance.mesh] = batch;
//    }
//
//    void pipeline::remove(mesh* mesh)
//    {
//        _meshesBatches[mesh]->remove(mesh);
//    }
//
//    void pipeline::update(const renderInstance& instance)
//    {
//        _meshesBatches[instance.mesh]->update(instance);
//    }
//
//    void pipeline::updateTranformBuffer(mesh* mesh, const mat4& modelMatrix)
//    {
//        _meshesBatches[mesh]->updateTransformBuffer(mesh, modelMatrix);
//    }
//
//    void pipeline::updateSelectionBuffer(mesh* mesh, bool isSelected)
//    {
//        _meshesBatches[mesh]->updateSelectionBuffer(mesh, isSelected);
//    }
//
//    void pipeline::update(const frameUniformBlock& frameUniformBlock)
//    {
//        _frameUniformBlockBuffer->subData(0, sizeof(phi::frameUniformBlock), &frameUniformBlock);
//        _frameUniformBlockBuffer->bindBufferBase(0);
//    }
//
//    void pipeline::render()
//    {
//    }
//}