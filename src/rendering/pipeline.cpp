#include "pipeline.h"
#include "shaderManager.h"
#include "materialGpuData.h"
#include "drawElementsIndirectCmd.h"

namespace phi
{
    void pipeline::init(pipelineInfo info)
    {
        initOpenGLExtensions();

        //_maxTexturesPerTextureArray = info.openGLconfig.maxTexturePerTextureArray; 
        _maxTexturesPerTextureArray = 20;
        _hasBindlessExtension = _openGLextensions["GL_ARB_bindless_texture"];

        setDefaultOpenGLStates(info.openGLconfig);

        createShader();

        createFrameUniformBlockBuffer(info.frameUniformBlock);
        createMaterialsBuffer(info.materials);
        createTextureArraysBuffer();
        createDrawCmdsBuffer(info.renderList);

        createVao(info.renderList);
    }

    void pipeline::initOpenGLExtensions()
    {
        const GLubyte* glExtension = nullptr;
        auto i = 0;

        glExtension = glGetStringi(GL_EXTENSIONS, i++);
        std::vector<std::string> glExtensions;

        while (glExtension != NULL)
        {
            glExtensions.push_back(std::string((char*)glExtension));
            glExtension = glGetStringi(GL_EXTENSIONS, i++);
        }

        std::vector<std::string> phiExtensions;
        phiExtensions.push_back("GL_ARB_bindless_texture");

        for (auto phiExtension : phiExtensions)
        {
            auto found = phi::contains(glExtensions, phiExtension);
            _openGLextensions[phiExtension] = found;
        }
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

            auto albedoTextureAddress = addTextureToArray(material->albedoTexture);
            auto normalTextureAddress = addTextureToArray(material->normalTexture);
            auto specularTextureAddress = addTextureToArray(material->specularTexture);
            auto emissiveTextureAddress = addTextureToArray(material->emissiveTexture);

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

    textureAddress pipeline::addTextureToArray(texture* tex)
    {
        auto texWidth = tex->w;
        auto texHeight = tex->h;
        auto it = find_if(_textureArrays.begin(), _textureArrays.end(), [&](textureArray* texArray)
        {
            auto arraySize = texArray->texCount;
            return texWidth == texArray->w &&
                texHeight == texArray->h;
        });

        textureArray* sameSizeArray = nullptr;

        if (it != _textureArrays.end())
            sameSizeArray = it[0];
        else
        {
            auto textureUnit = (GLint)_textureArrays.size();
            _textureArrayUnits.push_back(textureUnit);

            sameSizeArray = new textureArray(texWidth, texHeight, 44, textureUnit, _hasBindlessExtension);
            _textureArrays.push_back(sameSizeArray);
            sameSizeArray->loadOnGpu();
        }

        if (!sameSizeArray->hasTexture(tex))
        {
            sameSizeArray->add(tex);
            auto page = sameSizeArray->getTextureIndex(tex);
            _textureStorageData[tex].arrayIndex = (GLint)(find(_textureArrays.begin(), _textureArrays.end(), sameSizeArray) - _textureArrays.begin());
            _textureStorageData[tex].pageIndex = (GLfloat)page;
            return _textureStorageData[tex];
        }

        return _textureStorageData[tex];
    }

    void pipeline::createTextureArraysBuffer()
    {
        auto textureArraysCount = _textureArrays.size();

        if (_hasBindlessExtension)
        {
            GLuint64* textureArraysBufferData = new GLuint64[textureArraysCount];

            for (uint i = 0; i < textureArraysCount; i++)
                textureArraysBufferData[i] = _textureArrays[i]->handle;

            auto info = bufferDataInfo(textureArraysBufferData, GL_SHADER_STORAGE_BUFFER, textureArraysCount * sizeof(GLuint64), GL_STATIC_DRAW);
            auto textureArrayBuffer = new buffer(info);
            textureArrayBuffer->bindBufferBase(2);
            _buffers.push_back(textureArrayBuffer);
        }
        else
        {
            GLint* textureArraysBufferData = new GLint[textureArraysCount];

            for (uint i = 0; i < textureArraysCount; i++)
                textureArraysBufferData[i] = _textureArrayUnits[i];

            auto info = bufferDataInfo(textureArraysBufferData, GL_SHADER_STORAGE_BUFFER, textureArraysCount * sizeof(GLuint64), GL_STATIC_DRAW);
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
        _buffers[0]->bufferSubData(&frameUniformBlock);
    }
}