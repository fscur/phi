#include <phi\rendering\renderer.h>
#include <phi\rendering\materialGpuData.h>

namespace phi
{
    bool renderer::init(renderInfo info)
    {
        createShader();
        populateBuffersData(info);
        createBuffers();

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

        auto renderList = info.renderList;

        for (auto pair : renderList)
        {
            _vboSize += pair.first->vboSize;
            _eboSize += pair.first->eboSize;
            _modelMatricesBufferSize += pair.second.size() * sizeof(mat4);
            ++_objectCount;
        }
        
        auto verticesCount = _vboSize / sizeof(vertex);
        auto indicesCount = _eboSize / sizeof(uint);
        auto modelMatricesCount = _modelMatricesBufferSize / sizeof(mat4);

        _vboData = new vertex[verticesCount];
        _eboData = new uint[indicesCount];
        _modelMatricesBufferData = new mat4[modelMatricesCount];
        _materialsIdsBufferData = new uint[materialsCount];
        _drawCmdsBuffersData = new drawElementsIndirectCmd*[BUFFER_SIZE];

        auto vboOffset = 0;
        auto eboOffset = 0;
        auto drawIndex = -1;
        auto objectIndex = -1;
        auto indicesOffset = 0;
        auto verticesOffset = 0;
        auto instancesOffset = 0;

        for (auto &pair : renderList)
        {
            auto geometry = *pair.first;
            auto vboSize = geometry.vboSize;
            memcpy(_vboData + vboOffset, geometry.vboData, vboSize);
            vboOffset += vboSize;

            auto eboSize = geometry.eboSize;
            memcpy(_eboData + vboOffset, geometry.eboData, eboSize);
            eboOffset += eboSize;

            auto meshes = pair.second;
            auto meshsSize = meshes.size();
            auto instanceCount = 0;

            for (auto i = 0; i < meshsSize; i++)
            {
                auto mesh = *(meshes[i]);
                _modelMatricesBufferData[++drawIndex] = mesh.getLocalModelMatrix();
                _materialsIdsBufferData[drawIndex] = materialsMaterialsGpu[mesh.material];

                instanceCount++;
            }

            auto drawCmd = drawElementsIndirectCmd();
            drawCmd.indicesCount = geometry.indicesCount;
            drawCmd.instanceCount = instanceCount;
            drawCmd.firstIndex = indicesOffset;
            drawCmd.baseVertex = verticesOffset;
            drawCmd.baseInstance = instancesOffset;

            indicesOffset += geometry.indicesCount;
            verticesOffset += geometry.verticesCount;
            instancesOffset += instanceCount;

            _drawCmdsBuffersData[0][++objectIndex] = drawCmd;
        }

        _materialsBufferData = &materialsGpu[0];

        populateStaticDrawDataBuffer(info.staticDrawData);
    }

    void renderer::populateStaticDrawDataBuffer(staticDrawData data)
    {
        _staticDrawDataBufferSize = sizeof(staticDrawData);
        _staticDrawDataBufferData = new staticDrawData[1];
        _staticDrawDataBufferData[0] = data;
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

        _shader = shaderManager::get()->loadShader("shader.vert", "shader.frag", attribs);
    }

    void renderer::createBuffers()
    {
        createVao();
        createVbo();
        createMaterialsIdsBuffer();
        createModelMatricesBuffer();
        createEbo();
        createMaterialsBuffer();
        createDrawCmdsBuffers();
        createStaticDrawDataBuffer();
    }

    void renderer::createVao()
    {
        glCreateVertexArrays(1, &_vaoId);
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
        attribs.push_back(vertexAttrib(4, 1, GL_UNSIGNED_INT, sizeof(GLuint), 0, 1));

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
            attribs.push_back(vertexAttrib(5 + i, 4, sizeof(glm::mat4), sizeof(GLfloat) * i * 4, 1));

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
        _materialsBuffer->BindBufferBase(0);
    }

    void renderer::createDrawCmdsBuffers()
    {
        _drawCmdsBuffers = new buffer*[BUFFER_SIZE];

        for (uint i = 0; i < BUFFER_SIZE; i++)
        {
            auto info = bufferDataInfo(_drawCmdsBuffersData[i], GL_DRAW_INDIRECT_BUFFER, _drawCmdsBuffersSize, GL_STATIC_DRAW);
            _drawCmdsBuffers[i] = new buffer(info);
        }
    }

    void renderer::createStaticDrawDataBuffer()
    {
        auto info = bufferDataInfo(_staticDrawDataBufferData, GL_UNIFORM_BUFFER, _staticDrawDataBufferSize, GL_STATIC_DRAW);
        _staticDrawDataBuffer = new buffer(info);
        _staticDrawDataBuffer->BindBufferBase(1);
    }

    void renderer::createDefaultOpenGLStates()
    {
        glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }

    void renderer::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _drawCmdsBuffersIds[_drawRange]);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _objectCount, 0);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

        _lastDrawRange = _drawRange;
        _drawRange = ++_drawRange % BUFFER_SIZE;
    }
}