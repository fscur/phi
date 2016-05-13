#include <precompiled.h>
#include "pipeline.h"
#include "materialGpuData.h"
#include "drawElementsIndirectCmd.h"

namespace phi
{
    pipeline::pipeline(const gl* gl) :
        _gl(gl)
    {
        createFrameUniformBlockBuffer();
        createMaterialsBuffer();
    }

    pipeline::~pipeline()
    {
        safeDelete(_materialsBuffer);
        safeDelete(_frameUniformBlockBuffer);

        for (auto batch : batches)
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

    void pipeline::updateBatches(node* node)
    {
        //auto mesh = node->getComponent<phi::mesh>();
        //if (mesh)
        //{
        //    auto batch = _meshesBatches[mesh];
        //    _nodesToUpdate[batch].push_back(node);
        //}

        //auto children = node->getChildren();
        //for (auto child : *children)
        //    updateBatches(child);
    }

    void pipeline::add(const materialInstance& material)
    {
        _materialsBuffer->subData(material.offset, sizeof(phi::materialGpuData), &material.materialData);
        _materialsBuffer->bindBufferBase(1);
    }

    void pipeline::add(const renderInstance& instance)
    {
        auto i = 0u;
        auto added = false;
        auto batchesCount = batches.size();

        phi::batch* batch = nullptr;

        while (!added && i < batchesCount)
        {
            batch = batches[i++];
            added = batch->add(instance);
        }

        if (!added)
        {
            batch = new phi::batch();
            batches.push_back(batch);
            batch->add(instance);
        }

        _meshesBatches[instance.mesh] = batch;
    }

    void pipeline::remove(const renderInstance& instance)
    {
        //node->traverse<mesh>([&](mesh* mesh)
        //{
        //    auto it = _meshesBatches.find(mesh);
        //    if (it != _meshesBatches.end())
        //    {
        //        it->second->remove(mesh);
        //    }
        //});
    }

    void pipeline::update(const renderInstance& instance)
    {
        /*updateBatches(node);

        for (auto batch : batches)
        {
            vector<renderInstance> renderInstancesToUpdate;

            for (auto n : _nodesToUpdate[batch])
            {
                auto mesh = n->getComponent<phi::mesh>();
                auto instance = renderInstance();
                instance.mesh = mesh;
                instance.materialId = _materialsMaterialsGpu[mesh->material];
                instance.modelMatrix = n->getTransform()->getModelMatrix();
                renderInstancesToUpdate.push_back(instance);
            }

            batch->update(renderInstancesToUpdate);

            _nodesToUpdate[batch].clear();
        }*/
    }

    void pipeline::update(const frameUniformBlock& frameUniformBlock)
    {
        _frameUniformBlockBuffer->subData(0, sizeof(phi::frameUniformBlock), &frameUniformBlock);
        _frameUniformBlockBuffer->bindBufferBase(0);
    }
}