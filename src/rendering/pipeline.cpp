#include "pipeline.h"
#include "shaderManager.h"
#include "materialGpuData.h"
#include "drawElementsIndirectCmd.h"
#include <GL\glew.h>

namespace phi
{
    void pipeline::init(pipelineInfo info)
    {
        _textureManager = new textureManager();

        createFrameUniformBlockBuffer();
        createMaterialsBuffer(info.materials);
        _textureManager->load();
        createShader();

        _currentBatch = new batch();
        batches.push_back(_currentBatch);
        //createVao(info.renderList);
    }

    void pipeline::createShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");
        attribs.push_back("inNormal");
        attribs.push_back("inTangent");
        attribs.push_back("inMaterialId");
        attribs.push_back("inModelMatrix");

        _shader = shaderManager::get()->loadShader("basic.vert", "basic.frag", attribs);
        _shader->addUniform(0, "textureArrays");
        _shader->bind();

        if (phi::gl::currentState->useBindlessTextures)
            _shader->setUniform(0, _textureManager->handles);
        else
            _shader->setUniform(0, _textureManager->units);
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

    void pipeline::createMaterialsBuffer(std::vector<material*> materials)
    {
        auto materialsGpu = std::vector<materialGpuData>();

        auto materialsCount = materials.size();
        for (auto i = 0; i < materialsCount; i++)
        {
            auto material = materials[i];

            auto albedoTextureAddress = _textureManager->add(material->albedoTexture);
            auto normalTextureAddress = _textureManager->add(material->normalTexture);
            auto specularTextureAddress = _textureManager->add(material->specularTexture);
            auto emissiveTextureAddress = _textureManager->add(material->emissiveTexture);

            auto materialGpuData = phi::materialGpuData(
                albedoTextureAddress, 
                normalTextureAddress,
                specularTextureAddress,
                emissiveTextureAddress,
                material->albedoColor,
                material->specularColor,
                material->emissiveColor,
                material->shininess,
                material->reflectivity,
                material->emission,
                material->opacity);

            _materialsMaterialsGpu[material] = i;
            materialsGpu.push_back(materialGpuData);
        }

        auto materialsBufferSize = materialsCount * sizeof(materialGpuData);
        _materialsBuffer = new buffer(bufferTarget::shader);
        _materialsBuffer->storage(materialsBufferSize, &materialsGpu[0], bufferStorageUsage::write);
        _materialsBuffer->bindBufferBase(1);
    }

    void pipeline::updateFrameUniformBlock(phi::frameUniformBlock frameUniformBlock)
    {
        _frameUniformBlockBuffer->subData(0, sizeof(phi::frameUniformBlock), &frameUniformBlock);
    }

    void pipeline::addToBatch(object3D* object)
    {
        if (object->getType() == object3D::objectType::MESH)
        {
            auto mesh = static_cast<phi::mesh*>(object);
            auto material = mesh->material;
            auto geometry = mesh->geometry;

            phi::log(geometry);

            auto batchObject = phi::batchObject();
            batchObject.geometry = geometry;
            batchObject.materialId = _materialsMaterialsGpu[material];
            batchObject.modelMatrix = mesh->getModelMatrix();

            auto i = 0;
            auto added = false;
            auto batchesCount = batches.size();

            while (!added && i < batchesCount)
                added = batches[i++]->add(batchObject);

            if (!added)
            {
                auto batch = new phi::batch();
                batches.push_back(batch);
                batch->add(batchObject);
            }
        }

        auto children = object->getChildren();

        for (auto child : children)
            addToBatch(child);
    }

    void pipeline::add(object3D* object)
    {
        addToBatch(object);
    }
}