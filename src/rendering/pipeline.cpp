#include "pipeline.h"
#include "shaderManager.h"
#include "materialGpuData.h"
#include "drawElementsIndirectCmd.h"

namespace phi
{
    void pipeline::init(pipelineInfo info)
    {
        createShader();

        createMaterialsBuffer(info.materials);
        createDrawCmdsBuffer(info.renderList);
        createFrameUniformsBuffer(info.frameUniformBlock);

        createVao(info.renderList);

        setDefaultOpenGLStates(info.config);
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
        //_shader->addUniform(0, "vp");
        _shader->bind();
    }

    void pipeline::createMaterialsBuffer(std::vector<material*> materials)
    {
        auto materialsGpu = std::vector<materialGpuData>();

        auto materialsCount = materials.size();
        for (auto i = 0; i < materialsCount; i++)
        {
            auto material = materials[i];
            _materialsMaterialsGpu[material] = i;
            materialsGpu.push_back(materialGpuData::fromMaterial(*material));
        }

        auto materialsBufferSize = materialsCount * sizeof(materialGpuData);
        auto info = bufferDataInfo(&materialsGpu[0], GL_SHADER_STORAGE_BUFFER, materialsBufferSize, GL_STATIC_DRAW);

        auto materialsBuffer = new buffer(info);
        materialsBuffer->bindBufferBase(0);

        _buffers.push_back(materialsBuffer);
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

    void pipeline::createFrameUniformsBuffer(phi::frameUniformBlock frameUniformBlock)
    {
        auto info = bufferDataInfo(&frameUniformBlock, GL_UNIFORM_BUFFER, sizeof(phi::frameUniformBlock), GL_STATIC_DRAW);
        auto frameUniformsBuffer = new buffer(info);
        frameUniformsBuffer->bindBufferBase(1);

        _buffers.push_back(frameUniformsBuffer);
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

    void pipeline::setDefaultOpenGLStates(phi::gl::config config)
    {
        glClearColor(config.clearColor.r, config.clearColor.g, config.clearColor.b, config.clearColor.a);

        if (config.culling)
            glEnable(GL_CULL_FACE);

        auto cullBackFace = config.cullFace == phi::gl::cullFace::back;
        glCullFace(cullBackFace ? GL_BACK: GL_FRONT);

        auto frontFaceCCW = config.frontFace == phi::gl::frontFace::ccw;
        glFrontFace(frontFaceCCW ? GL_CCW : GL_CW);

        if (config.depthTest)
            glEnable(GL_DEPTH_TEST);

        auto depthMask = config.depthMask ? GL_TRUE : GL_FALSE;
        glDepthMask(depthMask);
    }

    void pipeline::updateFrameUniformBlock(phi::frameUniformBlock frameUniformBlock)
    {
        _buffers[2]->bufferSubData(&frameUniformBlock);
    }
}