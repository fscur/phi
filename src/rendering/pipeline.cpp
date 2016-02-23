#include "pipeline.h"
#include "materialGpuData.h"
#include "drawElementsIndirectCmd.h"

#include <GL\glew.h>

namespace phi
{
    pipeline::pipeline(phi::gl* gl) :
        _gl(gl)
    {
        createFrameUniformBlockBuffer();
        createMaterialsBuffer();
    }

    pipeline::~pipeline()
    {
        delete _materialsBuffer;
        delete _frameUniformBlockBuffer;

        auto batchesCount = batches.size();

        for (auto i = 0; i < batchesCount; ++i)
            delete batches[i];
    }

    void pipeline::createFrameUniformBlockBuffer()
    {
        _frameUniformBlockBuffer = new buffer(bufferTarget::uniform);

        _frameUniformBlockBuffer->storage(
            sizeof(phi::frameUniformBlock),
            nullptr,
            bufferStorageUsage::dynamic | bufferStorageUsage::write);

        _frameUniformBlockBuffer->bindBufferBase(0);
    }

    void pipeline::createMaterialsBuffer()
    {
        auto materialsBufferSize = MAX_MATERIALS_COUNT * sizeof(materialGpuData);
        _materialsBuffer = new buffer(bufferTarget::shader);
        _materialsBuffer->storage(materialsBufferSize, nullptr, bufferStorageUsage::dynamic | bufferStorageUsage::write);
        _materialsBuffer->bindBufferBase(1);
    }

    void pipeline::updateFrameUniformBlock(phi::frameUniformBlock frameUniformBlock)
    {
        _frameUniformBlockBuffer->subData(0, sizeof(phi::frameUniformBlock), &frameUniformBlock);
    }

    void pipeline::addToBatches(object3D* object)
    {
        if (object->getType() == object3D::objectType::MESH)
        {
            auto mesh = static_cast<phi::mesh*>(object);
            auto material = mesh->material;

            if (!phi::contains(_loadedMaterials, material))
                uploadMaterial(material);

            auto geometry = mesh->geometry;
            auto batchObject = phi::batchObject();
            batchObject.mesh = mesh;
            batchObject.geometry = geometry;
            batchObject.materialId = _materialsMaterialsGpu[material];
            batchObject.modelMatrix = mesh->getModelMatrix();

            addToBatches(batchObject);
        }

        auto children = object->getChildren();

        for (auto child : children)
            addToBatches(child);
    }

    void pipeline::addToBatches(batchObject& batchObject)
    {
        auto i = 0;
        auto added = false;
        auto batchesCount = batches.size();

        phi::batch* batch;

        while (!added && i < batchesCount)
        {
            batch = batches[i++];
            added = batch->add(batchObject);
        }

        if (!added)
        {
            batch = new phi::batch();
            batches.push_back(batch);
            batch->add(batchObject);
        }

        _meshesBatches[batchObject.mesh] = batch;
    }

    void pipeline::uploadMaterial(material* material)
    {
        if (_materialsMaterialsGpu.find(material) == _materialsMaterialsGpu.end())
            _materialsMaterialsGpu[material] = static_cast<uint>(_materialsMaterialsGpu.size());

        auto texturesManager = _gl->texturesManager;

        auto albedoTextureAddress = texturesManager->add(material->albedoTexture);
        auto normalTextureAddress = texturesManager->add(material->normalTexture);
        auto specularTextureAddress = texturesManager->add(material->specularTexture);
        auto emissiveTextureAddress = texturesManager->add(material->emissiveTexture);

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

        auto offset = _materialsMaterialsGpu[material] * sizeof(phi::materialGpuData);

        _materialsBuffer->subData(offset, sizeof(phi::materialGpuData), &materialGpuData);

        _loadedMaterials.push_back(material);
    }

    void pipeline::add(object3D* object)
    {
        addToBatches(object);
    }

    void pipeline::updateBatches(object3D* object)
    {
        if (object->getType() == object3D::objectType::MESH)
        {
            auto mesh = static_cast<phi::mesh*>(object);
            auto batch = _meshesBatches[mesh];
            _meshesToUpdate[batch].push_back(mesh);
        }

        auto children = object->getChildren();

        for (auto child : children)
            updateBatches(child);
    }

    void pipeline::update(object3D* object)
    {
        updateBatches(object);

        for (auto batch : batches)
        {
            std::vector<batchObject> bacthObjectsToUpdade;

            for (auto mesh : _meshesToUpdate[batch])
            {
                auto batchObject = phi::batchObject();
                batchObject.mesh = mesh;
                batchObject.geometry = mesh->geometry;
                batchObject.materialId = _materialsMaterialsGpu[mesh->material];
                batchObject.modelMatrix = mesh->getModelMatrix();
                bacthObjectsToUpdade.push_back(batchObject);
            }

            batch->update(bacthObjectsToUpdade);

            _meshesToUpdate[batch].clear();
        }
    }
}