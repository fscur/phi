#include "pipeline.h"
#include "shaderManager.h"
#include "materialGpuData.h"
#include "drawElementsIndirectCmd.h"

namespace phi
{
    void pipeline::init(pipelineInfo info)
    {
        _textureManager = new textureManager();

        createFrameUniformBlockBuffer(info.frameUniformBlock);
        createMaterialsBuffer(info.materials);
        //createTextureArraysBuffer();
        _textureManager->load();
        createDrawCmdsBuffer(info.renderList);

        createVao(info.renderList);

        createShader();
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

    void pipeline::createFrameUniformBlockBuffer(phi::frameUniformBlock frameUniformBlock)
    {
        auto info = bufferDataInfo(&frameUniformBlock, GL_UNIFORM_BUFFER, sizeof(phi::frameUniformBlock), GL_STATIC_DRAW);
        auto frameUniformsBuffer = new buffer(info);
        frameUniformsBuffer->bindBufferBase(0);

        _buffers.push_back(frameUniformsBuffer);
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
        auto info = bufferDataInfo(&materialsGpu[0], GL_SHADER_STORAGE_BUFFER, materialsBufferSize, GL_STATIC_DRAW);

        auto materialsBuffer = new buffer(info);
        materialsBuffer->bindBufferBase(1);

        _buffers.push_back(materialsBuffer);
    }

    void pipeline::createTextureArraysBuffer()
    {
        if (_hasBindlessExtension)
        {
            auto info = bufferDataInfo(&(_textureManager->handles[0]), GL_SHADER_STORAGE_BUFFER, _textureManager->handles.size() * sizeof(GLuint64), GL_STATIC_DRAW);
            auto textureArrayBuffer = new buffer(info);
            textureArrayBuffer->bindBufferBase(2);
            _buffers.push_back(textureArrayBuffer);
        }
        else
        {
            auto info = bufferDataInfo(&(_textureManager->units[0]), GL_SHADER_STORAGE_BUFFER, _textureManager->units.size() * sizeof(GLint), GL_STATIC_DRAW);
            auto textureArrayBuffer = new buffer(info);
            textureArrayBuffer->bindBufferBase(2);
            _buffers.push_back(textureArrayBuffer);
        }
    }

    void pipeline::createDrawCmdsBuffer(std::map<geometry*, std::vector<mesh*>> renderList)
    {
        auto indicesOffset = 0;
        auto verticesOffset = 0;
        auto instancesOffset = 0;
        std::vector<drawElementsIndirectCmd> drawCmdsBufferData;

        for (auto &pair : renderList)
        {
            auto geometry = pair.first;
            GLuint instanceCount = static_cast<GLuint>(pair.second.size());
            GLuint indicesCount = geometry->indicesCount;

            auto drawCmd = drawElementsIndirectCmd();
            drawCmd.indicesCount = indicesCount;
            drawCmd.instanceCount = instanceCount;
            drawCmd.firstIndex = indicesOffset;
            drawCmd.baseVertex = verticesOffset;
            drawCmd.baseInstance = instancesOffset;

            indicesOffset += indicesCount;
            verticesOffset += geometry->verticesCount;
            instancesOffset += instanceCount;

            drawCmdsBufferData.push_back(drawCmd);
        }

        auto info = bufferDataInfo(&drawCmdsBufferData[0], GL_DRAW_INDIRECT_BUFFER, drawCmdsBufferData.size() * sizeof(drawElementsIndirectCmd), GL_STATIC_DRAW);
        auto drawCmdsBuffer = new buffer(info);
        drawCmdsBuffer->bind();

        _buffers.push_back(drawCmdsBuffer);
    }

    void pipeline::createVao(std::map<geometry*, std::vector<mesh*>> renderList)
    {
        GLuint vboSize = 0;
        GLuint eboSize = 0;
        std::vector<mat4> modelMatrices;
        uint drawCount = 0;

        uint objectsCount = 0;

        for (auto &pair : renderList)
        {
            vboSize += pair.first->vboSize;
            eboSize += pair.first->eboSize;
            drawCount += (uint)pair.second.size();
            ++objectsCount;
        }

        vao = new vertexArrayObject(objectsCount);
        vao->bind();

        auto verticesCount = vboSize / sizeof(vertex);
        auto indicesCount = eboSize / sizeof(uint);

        auto vboData = new vertex[verticesCount];
        auto eboData = new uint[indicesCount];
        auto materialsIdsBufferData = new uint[drawCount];

        auto vboOffset = 0;
        auto eboOffset = 0;
        auto drawIndex = -1;

        for (auto &pair : renderList)
        {
            auto geometry = pair.first;
            auto vboSize = geometry->vboSize;
            memcpy(vboData + vboOffset, geometry->vboData, vboSize);
            vboOffset += (vboSize / sizeof(vertex));

            auto eboSize = geometry->eboSize;
            memcpy(eboData + eboOffset, geometry->eboData, eboSize);
            eboOffset += (eboSize / sizeof(uint));

            auto meshes = pair.second;
            auto meshesCount = meshes.size();

            for (auto i = 0; i < meshesCount; i++)
            {
                auto mesh = meshes[i];
                auto modelMatrix = mesh->getModelMatrix();
                modelMatrices.push_back(modelMatrix);
                materialsIdsBufferData[++drawIndex] = _materialsMaterialsGpu[mesh->material];
            }
        }

        vao->createVbo(vboData, vboSize);
        vao->createMaterialsIdsBuffer(materialsIdsBufferData, drawCount * sizeof(uint));
        vao->createModelMatricesBuffer(&modelMatrices[0], drawCount * sizeof(mat4));
        vao->createEbo(eboData, eboSize);
    }

    void pipeline::updateFrameUniformBlock(phi::frameUniformBlock frameUniformBlock)
    {
        _buffers[0]->bufferSubData(&frameUniformBlock);
    }
}