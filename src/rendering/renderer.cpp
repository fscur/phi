#include <phi\rendering\renderer.h>
#include <phi\rendering\materialGpuData.h>

namespace phi
{
    bool renderer::init(renderInfo info)
    {
        createShader();

        createMaterialsBuffer(info.materials);
        createDrawCmdsBuffer(info.renderList);
        createFrameUniformsBuffer(info.frameUniformBlock);

        createVao(info.renderList);

        createDefaultOpenGLStates();

        return true;
    }

    void renderer::createShader()
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

    void renderer::createMaterialsBuffer(std::vector<material*> materials)
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
        _materialsBuffer = new buffer(info);
        _materialsBuffer->bindBufferBase(0);
    }

    void renderer::createDrawCmdsBuffer(std::map<geometry*, std::vector<mesh*>> renderList)
    {
        auto indicesOffset = 0;
        auto verticesOffset = 0;
        auto instancesOffset = 0;
        std::vector<drawElementsIndirectCmd> drawCmdsBufferData;

        for (auto &pair : renderList)
        {
            auto geometry = pair.first;
            auto instanceCount = pair.second.size();
            auto indicesCount = geometry->indicesCount;

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
        _drawCmdsBuffer = new buffer(info);
        _drawCmdsBuffer->bind();
    }

    void renderer::createFrameUniformsBuffer(phi::frameUniformBlock frameUniformBlock)
    {
        auto info = bufferDataInfo(&frameUniformBlock, GL_UNIFORM_BUFFER, sizeof(phi::frameUniformBlock), GL_STATIC_DRAW);
        _frameUniformsBuffer = new buffer(info);
        _frameUniformsBuffer->bindBufferBase(1);
    }

    void renderer::createVao(std::map<geometry*, std::vector<mesh*>> renderList)
    {
        glCreateVertexArrays(1, &_vaoId);
        glBindVertexArray(_vaoId);

        GLuint vboSize = 0;
        GLuint eboSize = 0;
        std::vector<mat4> modelMatrices;
        uint drawCount = 0;

        for (auto &pair : renderList)
        {
            vboSize += pair.first->vboSize;
            eboSize += pair.first->eboSize;
            drawCount += pair.second.size();
            ++_objectCount;
        }

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

        createVbo(vboData, vboSize);
        createMaterialsIdsBuffer(materialsIdsBufferData, drawCount * sizeof(uint));
        createModelMatricesBuffer(&modelMatrices[0], drawCount * sizeof(mat4));
        createEbo(eboData, eboSize);
    }

    void renderer::createVbo(void* data, GLsizeiptr size)
    {
        std::vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), 0));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), sizeof(float) * 3));
        attribs.push_back(vertexAttrib(2, 3, GL_FLOAT, sizeof(vertex), sizeof(float) * 5));
        attribs.push_back(vertexAttrib(3, 3, GL_FLOAT, sizeof(vertex), sizeof(float) * 8));

        vertexBufferDataInfo info;
        info.data = data;
        info.size = size;
        info.flags = GL_STATIC_DRAW;

        _vbo = new vertexBuffer(info, attribs);
    }

    void renderer::createMaterialsIdsBuffer(void* data, GLsizeiptr size)
    {
        std::vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(4, 1, GL_UNSIGNED_INT, 0, 0, 1));

        vertexBufferDataInfo info;
        info.data = data;
        info.size = size;
        info.flags = GL_STATIC_DRAW;

        _materialsIdsBuffer = new vertexBuffer(info, attribs);
    }

    void renderer::createModelMatricesBuffer(void* data, GLsizeiptr size)
    {
        std::vector<vertexAttrib> attribs;

        for (uint i = 0; i < 4; i++)
            attribs.push_back(vertexAttrib(5 + i, 4, GL_FLOAT, sizeof(glm::mat4), sizeof(GLfloat) * i * 4, 1));

        vertexBufferDataInfo info;
        info.data = data;
        info.size = size;
        info.flags = GL_STATIC_DRAW;

        _modelMatricesBuffer = new vertexBuffer(info, attribs);
    }

    void renderer::createEbo(void* data, GLsizeiptr size)
    {
        auto info = elementBufferDataInfo(data, size, GL_STATIC_DRAW);
        _ebo = new elementBuffer(info);
    }

    void renderer::createDefaultOpenGLStates()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 0.5f);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
    }

    void renderer::updateFrameUniformsBuffer(phi::frameUniformBlock frameUniformBlock)
    {
        _frameUniformsBuffer->bufferSubData(&frameUniformBlock);
    }

    void renderer::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _objectCount, 0);
    }
}