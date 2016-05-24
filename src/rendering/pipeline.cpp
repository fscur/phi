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

        for (auto pair : _imageTextures)
            safeDelete(pair.second);
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

        auto albedoTexture = getMaterialTexture(material->albedoImage, _gl->defaultAlbedoImage);
        auto normalTexture = getMaterialTexture(material->normalImage, _gl->defaultNormalImage);
        auto specularTexture = getMaterialTexture(material->specularImage, _gl->defaultSpecularImage);
        auto emissiveTexture = getMaterialTexture(material->emissiveImage, _gl->defaultEmissiveImage);

        auto albedoTextureAddress = _gl->texturesManager->add(albedoTexture);
        auto normalTextureAddress = _gl->texturesManager->add(normalTexture);
        auto specularTextureAddress = _gl->texturesManager->add(specularTexture);
        auto emissiveTextureAddress = _gl->texturesManager->add(emissiveTexture);

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

    void pipeline::add(mesh* mesh, mat4 modelMatrix)
    {
        if (mesh->material == nullptr)
            mesh->material = _gl->defaultMaterial;

        uploadMaterialIfNew(mesh->material);

        renderInstance instance = { 0 };
        instance.mesh = mesh;
        instance.modelMatrix = modelMatrix;
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

    texture* pipeline::getMaterialTexture(image* materialImage, phi::image* defaultImage)
    {
        phi::texture* texture = nullptr;

        if (materialImage == nullptr)
            materialImage = defaultImage;

        if (_imageTextures.find(materialImage) != _imageTextures.end())
        {
            texture = _imageTextures[materialImage];
        }
        else
        {
            texture = new phi::texture(
                materialImage,
                GL_TEXTURE_2D,
                GL_RGBA8,
                GL_REPEAT,
                GL_LINEAR_MIPMAP_LINEAR,
                GL_LINEAR,
                true);

            _imageTextures[materialImage] = texture;
        }

        return texture;
    }

    void pipeline::remove(mesh* mesh)
    {
        _meshesBatches[mesh]->remove(mesh);
    }

    void pipeline::update(const renderInstance& instance)
    {
        _meshesBatches[instance.mesh]->update(instance);
    }

    void pipeline::updateTranformBuffer(mesh* mesh, const mat4& modelMatrix)
    {
        _meshesBatches[mesh]->updateTransformBuffer(mesh, modelMatrix);
    }

    void pipeline::updateSelectionBuffer(mesh* mesh, bool isSelected)
    {
        _meshesBatches[mesh]->updateSelectionBuffer(mesh, isSelected);
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