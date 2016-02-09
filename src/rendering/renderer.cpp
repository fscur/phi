#include <phi\rendering\renderer.h>
#include <phi\rendering\materialGpuData.h>

namespace phi
{
    bool renderer::init(renderInfo info)
    {
        _frameUniformsBufferData = info.frameUniformsBufferData;
        createShader();
        populateBuffersData(info);
        createBuffers();
        createDefaultOpenGLStates();
        //_bufferLockManager = new bufferLockManager(true);
        return true;
    }

    void renderer::populateBuffersData(renderInfo info)
    {
        auto materials = info.materials;
        auto materialsGpu = std::vector<materialGpuData>();
        auto materialsMaterialsGpu = std::map<material*, uint>();

        auto materialsCount = materials.size();
        for (auto i = 0; i < materialsCount; i++)
        {
            auto material = materials[i];
            materialsMaterialsGpu[material] = i;
            materialsGpu.push_back(materialGpuData::fromMaterial(*material));
        }

        _materialsBufferSize = materialsCount * sizeof(materialGpuData);
        _materialsBufferData = new materialGpuData[materialsCount];
        memcpy(_materialsBufferData, &materialsGpu[0], _materialsBufferSize);

        auto renderList = info.renderList;
        
        for (auto &pair : renderList)
        {
            _vboSize += pair.first->vboSize;
            _eboSize += pair.first->eboSize;
            _drawCount += pair.second.size();
            ++_objectCount;
        }
        
        auto verticesCount = _vboSize / sizeof(vertex);
        auto indicesCount = _eboSize / sizeof(uint);
        _modelMatricesBufferSize = _drawCount * sizeof(mat4);

        _vboData = new vertex[verticesCount];
        _eboData = new uint[indicesCount];
        _modelMatricesBufferData = new mat4[_drawCount];
        _materialsIdsBufferData = new uint[_drawCount];
        _drawCmdsBuffersData = new drawElementsIndirectCmd*[BUFFER_SIZE];
        
        for (auto i = 0; i < BUFFER_SIZE; i++)
            _drawCmdsBuffersData[i] = new drawElementsIndirectCmd[_objectCount];

        auto vboOffset = 0;
        auto eboOffset = 0;
        auto drawIndex = -1;
        auto objectIndex = -1;
        auto indicesOffset = 0;
        auto verticesOffset = 0;
        auto instancesOffset = 0;

        for (auto &pair : renderList)
        {
            auto geometry = pair.first;
            auto vboSize = geometry->vboSize;
            memcpy(_vboData + vboOffset, geometry->vboData, vboSize);
            vboOffset += (vboSize / sizeof(vertex));

            auto eboSize = geometry->eboSize;
            memcpy(_eboData + eboOffset, geometry->eboData, eboSize);
            eboOffset += (eboSize / sizeof(uint));

            auto meshes = pair.second;
            auto meshsSize = meshes.size();
            auto instanceCount = 0;

            for (auto i = 0; i < meshsSize; i++)
            {
                auto mesh = meshes[i];
                auto modelMatrix = mesh->getModelMatrix();
                _modelMatricesBufferData[++drawIndex] = modelMatrix;
                _materialsIdsBufferData[drawIndex] = materialsMaterialsGpu[mesh->material];

                instanceCount++;
            }

            auto drawCmd = drawElementsIndirectCmd();
            drawCmd.indicesCount = geometry->indicesCount;
            drawCmd.instanceCount = instanceCount;
            drawCmd.firstIndex = indicesOffset;
            drawCmd.baseVertex = verticesOffset;
            drawCmd.baseInstance = instancesOffset;

            indicesOffset += geometry->indicesCount;
            verticesOffset += geometry->verticesCount;
            instancesOffset += instanceCount;

            ++objectIndex;

            for (auto i = 0; i < BUFFER_SIZE; i++)
            {
                _drawCmdsBuffersData[i][objectIndex] = drawCmd;
            }
        }

        _materialsIdsBufferSize = instancesOffset * sizeof(uint);
        _drawCmdsBuffersSize = _objectCount * sizeof(drawElementsIndirectCmd);
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

    void renderer::createBuffers()
    {
        createVao();
        createVbo();
        createMaterialsIdsBuffer();
        createModelMatricesBuffer();
        createEbo();
        createMaterialsBuffer();
        createFrameUniformsBuffer();
        createDrawCmdsBuffers();
    }

    void renderer::createVao()
    {
        glCreateVertexArrays(1, &_vaoId);
        glBindVertexArray(_vaoId);
    }

    void renderer::createVbo()
    {
        std::vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), 0));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), sizeof(float) * 3));
        attribs.push_back(vertexAttrib(2, 3, GL_FLOAT, sizeof(vertex), sizeof(float) * 5));
        attribs.push_back(vertexAttrib(3, 3, GL_FLOAT, sizeof(vertex), sizeof(float) * 8));

        vertexBufferDataInfo info;
        info.data = _vboData;
        info.size = _vboSize;
        info.flags = GL_STATIC_DRAW;

        _vbo = new vertexBuffer(info, attribs);
    }

    void renderer::createMaterialsIdsBuffer()
    {
        std::vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(4, 1, GL_UNSIGNED_INT, 0, 0, 1));

        vertexBufferDataInfo info;
        info.data = _materialsIdsBufferData;
        info.size = _materialsIdsBufferSize;
        info.flags = GL_STATIC_DRAW;

        _materialsIdsBuffer = new vertexBuffer(info, attribs);
    }

    void renderer::createModelMatricesBuffer()
    {
        std::vector<vertexAttrib> attribs;

        for (uint i = 0; i < 4; i++)
            attribs.push_back(vertexAttrib(5 + i, 4, GL_FLOAT, sizeof(glm::mat4), sizeof(GLfloat) * i * 4, 1));

        vertexBufferDataInfo info;
        info.data = _modelMatricesBufferData;
        info.size = _modelMatricesBufferSize;
        info.flags = GL_STATIC_DRAW;

        _modelMatricesBuffer = new vertexBuffer(info, attribs);
    }

    void renderer::createEbo()
    {
        auto info = elementBufferDataInfo(_eboData, _eboSize, GL_STATIC_DRAW);
        _ebo = new elementBuffer(info);
    }

    void renderer::createMaterialsBuffer()
    {
        auto info = bufferDataInfo(_materialsBufferData, GL_SHADER_STORAGE_BUFFER, _materialsBufferSize, GL_STATIC_DRAW);
        _materialsBuffer = new buffer(info);
        _materialsBuffer->bindBufferBase(0);
    }

    void renderer::createDrawCmdsBuffers()
    {
        _drawCmdsBuffers = new buffer*[BUFFER_SIZE];

        for (uint i = 0; i < BUFFER_SIZE; i++)
        {
            for (auto j = 0; j < _objectCount; j++)
            {
                auto bufferData = _drawCmdsBuffersData[i][j];
                bufferData.firstIndex = bufferData.firstIndex * (i + 1);
            };

            auto info = bufferDataInfo(_drawCmdsBuffersData[i], GL_DRAW_INDIRECT_BUFFER, _drawCmdsBuffersSize, GL_STATIC_DRAW);
            _drawCmdsBuffers[i] = new buffer(info);
        }
    }

    void renderer::createFrameUniformsBuffer()
    {
        auto info = bufferDataInfo(&_frameUniformsBufferData, GL_UNIFORM_BUFFER, sizeof(frameUniformsBufferData), GL_STATIC_DRAW);
        _frameUniformsBuffer = new buffer(info);
        _frameUniformsBuffer->bindBufferBase(1);
    }

    void renderer::createDefaultOpenGLStates()
    {
        glClearColor(1.0f, 1.0f, 1.0f, 0.5f);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
    }

    void renderer::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        auto drawCmdsBuffer = _drawCmdsBuffers[0];
        drawCmdsBuffer->bind();
        //glDrawElements(GL_TRIANGLES, _eboSize / sizeof(uint), GL_UNSIGNED_INT, 0);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _objectCount, 0);
        drawCmdsBuffer->unbind();

        //_bufferLockManager.lockRange(_drawRange, 1);

        //_drawRange = ++_drawRange % BUFFER_SIZE;
    }
}