#include <precompiled.h>
#include "pipeline.h"
#include "materialGpuData.h"
#include "drawElementsIndirectCmd.h"

namespace phi
{
    pipeline::pipeline(gl* gl) :
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
        auto materialsBufferSize = MAX_MATERIALS_COUNT * sizeof(materialGpuData);
        _materialsBuffer = new buffer(bufferTarget::shader);
        _materialsBuffer->storage(materialsBufferSize, nullptr, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void pipeline::updateFrameUniformBlock(const frameUniformBlock& frameUniformBlock)
    {
        _frameUniformBlockBuffer->subData(0, sizeof(phi::frameUniformBlock), &frameUniformBlock);
        _frameUniformBlockBuffer->bindBufferBase(0);
    }

    void pipeline::addToBatches(node* node)
    {
        auto mesh = node->getComponent<phi::mesh>();

        if (mesh)
        {
            auto material = mesh->material;

			if (material == nullptr)
				material = _gl->defaultMaterial;

            if (!phi::contains(_loadedMaterials, material))
                uploadMaterial(material);

            auto geometry = mesh->geometry;
            auto batchObject = phi::batchObject();
            batchObject.mesh = mesh;
            batchObject.geometry = geometry;
            batchObject.materialId = _materialsMaterialsGpu[material];
            batchObject.modelMatrix = node->getTransform()->getModelMatrix();

            addToBatches(batchObject);

            _transformChangedTokens[node] = node->getTransformChanged()->assign(
                std::bind(
                    &pipeline::nodeTransformChanged,
                    this,
                    std::placeholders::_1));

            _selectionChangedTokens[mesh] = mesh->getSelectionChanged()->assign(
                std::bind(
                    &pipeline::meshSelectionChanged,
                    this,
                    std::placeholders::_1));
        }

        auto children = node->getChildren();

        for (auto child : *children)
            addToBatches(child);
    }

    void pipeline::addToBatches(const batchObject& batchObject)
    {
        auto i = 0u;
        auto added = false;
        auto batchesCount = batches.size();

        phi::batch* batch = nullptr;

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

	texture* pipeline::getMaterialTexture(image* materialImage, phi::image* defaultImage)
	{
		phi::texture* texture = nullptr;
		
		auto image = materialImage;
		if (image == nullptr)
			image = _gl->defaultAlbedoImage;

		if (_imageTextures.find(image) != _imageTextures.end())
			texture = _imageTextures[image];
		else
		{
			texture = new phi::texture(
				image,
				GL_TEXTURE_2D,
				GL_RGBA8,
				GL_REPEAT,
				GL_LINEAR_MIPMAP_LINEAR,
				GL_LINEAR,
				true);

			_imageTextures[image] = texture;
		}

		return texture;
	}

    void pipeline::uploadMaterial(material* material)
    {
        if (_materialsMaterialsGpu.find(material) == _materialsMaterialsGpu.end())
            _materialsMaterialsGpu[material] = static_cast<uint>(_materialsMaterialsGpu.size());

        auto texturesManager = _gl->texturesManager;

		texture* albedoTexture = getMaterialTexture(material->albedoImage, _gl->defaultAlbedoImage);
		texture* normalTexture = getMaterialTexture(material->normalImage, _gl->defaultNormalImage);
		texture* specularTexture = getMaterialTexture(material->specularImage, _gl->defaultSpecularImage);
		texture* emissiveTexture = getMaterialTexture(material->emissiveImage, _gl->defaultEmissiveImage);

        auto albedoTextureAddress = texturesManager->add(albedoTexture);
        auto normalTextureAddress = texturesManager->add(normalTexture);
        auto specularTextureAddress = texturesManager->add(specularTexture);
        auto emissiveTextureAddress = texturesManager->add(emissiveTexture);

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
        _materialsBuffer->bindBufferBase(1);
        _loadedMaterials.push_back(material);
    }

    void pipeline::add(node* node)
    {
        addToBatches(node);
    }

    void pipeline::remove(node* node)
    {
        node->traverse<mesh>([&](mesh* mesh)
        {
            auto it = _meshesBatches.find(mesh);
            if (it != _meshesBatches.end())
            {
                it->second->remove(mesh);
            }
        });
    }

    void pipeline::updateBatches(node* node)
    {
        auto mesh = node->getComponent<phi::mesh>();
        if (mesh)
        {
            auto batch = _meshesBatches[mesh];
            _nodesToUpdate[batch].push_back(node);
        }

        auto children = node->getChildren();
        for (auto child : *children)
            updateBatches(child);
    }

    void pipeline::update(node* node)
    {
        updateBatches(node);

        for (auto batch : batches)
        {
            vector<batchObject> bacthObjectsToUpdade;

            for (auto n : _nodesToUpdate[batch])
            {
                auto mesh = n->getComponent<phi::mesh>();
                auto batchObject = phi::batchObject();
                batchObject.mesh = mesh;
                batchObject.geometry = mesh->geometry;
                batchObject.materialId = _materialsMaterialsGpu[mesh->material];
                batchObject.modelMatrix = n->getTransform()->getModelMatrix();
                bacthObjectsToUpdade.push_back(batchObject);
            }

            batch->update(bacthObjectsToUpdade);

            _nodesToUpdate[batch].clear();
        }
    }

    void pipeline::nodeTransformChanged(node* sender)
    {
        auto mesh = sender->getComponent<phi::mesh>();
        auto modelMatrix = sender->getTransform()->getModelMatrix();
        auto batch = _meshesBatches[mesh];

        batch->updateModelMatricesBuffer(mesh, modelMatrix);
    }

    void pipeline::meshSelectionChanged(mesh* sender)
    {
        auto batch = _meshesBatches[sender];
        auto color = sender->getSelectionColor();

        batch->updateSelectionBuffer(sender, color);
    }
}