#include <precompiled.h>
#include "glslCompiler.h"

namespace phi
{
    using namespace glslang;

    EShLanguage glslCompiler::getLanguage(const string & name)
    {
        size_t ext = name.rfind('.');
        if (ext == string::npos)
        {
            return EShLangVertex;
        }

        string suffix = name.substr(ext + 1, string::npos);

        if (suffix == "vert")
            return EShLangVertex;
        else if (suffix == "tesc")
            return EShLangTessControl;
        else if (suffix == "tese")
            return EShLangTessEvaluation;
        else if (suffix == "geom")
            return EShLangGeometry;
        else if (suffix == "frag")
            return EShLangFragment;
        else if (suffix == "comp")
            return EShLangCompute;

        return EShLangVertex;
    }

    string glslCompiler::getDataType(int type)
    {
        switch (type)
        {
        case 5120: return "BYTE";
        case 5121: return "UNSIGNED_BYTE";
        case 5122: return "SHORT";
        case 5123: return "UNSIGNED_SHORT";
        case 5124: return "INT";
        case 5125: return "UNSIGNED_INT";
        case 5126: return "FLOAT";
        case 35664: return "FLOAT_VEC2";
        case 35665: return "FLOAT_VEC3";
        case 35666: return "FLOAT_VEC4";
        case 35667: return "INT_VEC2";
        case 35668: return "INT_VEC3";
        case 35669: return "INT_VEC4";
        case 35670: return "BOOL";
        case 35671: return "BOOL_VEC2";
        case 35672: return "BOOL_VEC3";
        case 35673: return "BOOL_VEC4";
        case 35674: return "FLOAT_MAT2";
        case 35675: return "FLOAT_MAT3";
        case 35676: return "FLOAT_MAT4";
        case 35678: return "SAMPLER_2D";
        default: return "";
        }
    }
    
    TBuiltInResource glslCompiler::createResources()
    {
        TBuiltInResource resources;
        resources.maxLights = 32;
        resources.maxClipPlanes = 6;
        resources.maxTextureUnits = 32;
        resources.maxTextureCoords = 32;
        resources.maxVertexAttribs = 64;
        resources.maxVertexUniformComponents = 4096;
        resources.maxVaryingFloats = 64;
        resources.maxVertexTextureImageUnits = 32;
        resources.maxCombinedTextureImageUnits = 80;
        resources.maxTextureImageUnits = 32;
        resources.maxFragmentUniformComponents = 4096;
        resources.maxDrawBuffers = 32;
        resources.maxVertexUniformVectors = 128;
        resources.maxVaryingVectors = 8;
        resources.maxFragmentUniformVectors = 16;
        resources.maxVertexOutputVectors = 16;
        resources.maxFragmentInputVectors = 15;
        resources.minProgramTexelOffset = -8;
        resources.maxProgramTexelOffset = 7;
        resources.maxClipDistances = 8;
        resources.maxComputeWorkGroupCountX = 65535;
        resources.maxComputeWorkGroupCountY = 65535;
        resources.maxComputeWorkGroupCountZ = 65535;
        resources.maxComputeWorkGroupSizeX = 1024;
        resources.maxComputeWorkGroupSizeY = 1024;
        resources.maxComputeWorkGroupSizeZ = 64;
        resources.maxComputeUniformComponents = 1024;
        resources.maxComputeTextureImageUnits = 16;
        resources.maxComputeImageUniforms = 8;
        resources.maxComputeAtomicCounters = 8;
        resources.maxComputeAtomicCounterBuffers = 1;
        resources.maxVaryingComponents = 60;
        resources.maxVertexOutputComponents = 64;
        resources.maxGeometryInputComponents = 64;
        resources.maxGeometryOutputComponents = 128;
        resources.maxFragmentInputComponents = 128;
        resources.maxImageUnits = 8;
        resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
        resources.maxCombinedShaderOutputResources = 8;
        resources.maxImageSamples = 0;
        resources.maxVertexImageUniforms = 0;
        resources.maxTessControlImageUniforms = 0;
        resources.maxTessEvaluationImageUniforms = 0;
        resources.maxGeometryImageUniforms = 0;
        resources.maxFragmentImageUniforms = 8;
        resources.maxCombinedImageUniforms = 8;
        resources.maxGeometryTextureImageUnits = 16;
        resources.maxGeometryOutputVertices = 256;
        resources.maxGeometryTotalOutputComponents = 1024;
        resources.maxGeometryUniformComponents = 1024;
        resources.maxGeometryVaryingComponents = 64;
        resources.maxTessControlInputComponents = 128;
        resources.maxTessControlOutputComponents = 128;
        resources.maxTessControlTextureImageUnits = 16;
        resources.maxTessControlUniformComponents = 1024;
        resources.maxTessControlTotalOutputComponents = 4096;
        resources.maxTessEvaluationInputComponents = 128;
        resources.maxTessEvaluationOutputComponents = 128;
        resources.maxTessEvaluationTextureImageUnits = 16;
        resources.maxTessEvaluationUniformComponents = 1024;
        resources.maxTessPatchComponents = 120;
        resources.maxPatchVertices = 32;
        resources.maxTessGenLevel = 64;
        resources.maxViewports = 16;
        resources.maxVertexAtomicCounters = 0;
        resources.maxTessControlAtomicCounters = 0;
        resources.maxTessEvaluationAtomicCounters = 0;
        resources.maxGeometryAtomicCounters = 0;
        resources.maxFragmentAtomicCounters = 8;
        resources.maxCombinedAtomicCounters = 8;
        resources.maxAtomicCounterBindings = 1;
        resources.maxVertexAtomicCounterBuffers = 0;
        resources.maxTessControlAtomicCounterBuffers = 0;
        resources.maxTessEvaluationAtomicCounterBuffers = 0;
        resources.maxGeometryAtomicCounterBuffers = 0;
        resources.maxFragmentAtomicCounterBuffers = 1;
        resources.maxCombinedAtomicCounterBuffers = 1;
        resources.maxAtomicCounterBufferSize = 16384;
        resources.maxTransformFeedbackBuffers = 4;
        resources.maxTransformFeedbackInterleavedComponents = 64;
        resources.maxCullDistances = 8;
        resources.maxCombinedClipAndCullDistances = 8;
        resources.maxSamples = 4;
        resources.limits.nonInductiveForLoops = 1;
        resources.limits.whileLoops = 1;
        resources.limits.doWhileLoops = 1;
        resources.limits.generalUniformIndexing = 1;
        resources.limits.generalAttributeMatrixVectorIndexing = 1;
        resources.limits.generalVaryingIndexing = 1;
        resources.limits.generalSamplerIndexing = 1;
        resources.limits.generalVariableIndexing = 1;
        resources.limits.generalConstantMatrixVectorIndexing = 1;

        return resources;
    }
    
    TShader * glslCompiler::parse(EShLanguage language, const string fileName)
    {
        auto shader = new TShader(language);
        auto messages = EShMessages::EShMsgDefault;
        auto cstr = fileName.c_str();
        shader->setStrings(&cstr, 1);

        auto resources = createResources();

        if (!shader->parse(&resources, 100, false, messages))
        {
            phi::debug(shader->getInfoLog());
            phi::debug(shader->getInfoDebugLog());
        }

        return shader;
    }

    glslang::TProgram glslCompiler::compile(const string vertFile, const string fragFile)
    {
        InitializeProcess();

        auto vertexShader = parse(EShLanguage::EShLangVertex, vertFile);
        auto fragmentShader = parse(EShLanguage::EShLangFragment, fragFile);
        auto messages = EShMessages::EShMsgDefault;
        auto program = TProgram();

        program.addShader(vertexShader);
        program.addShader(fragmentShader);

        if (!program.link(messages))
            phi::debug("glslCompiler::compile: unable to link program;");

        program.buildReflection();

        auto sizeU = program.getNumLiveUniformVariables();

        for (auto i = 0; i < sizeU; i++)
            phi::debug(program.getUniformName(i));

        sizeU = program.getNumLiveAttributes();

        for (auto i = 0; i < sizeU; i++)
            phi::debug(getDataType(program.getAttributeType(i)) + ": " + program.getAttributeName(i));

        program.dumpReflection();

        FinalizeProcess();

        return program;
    }
}