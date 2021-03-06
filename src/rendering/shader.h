#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\color.h>

#include "shaderCompilationResult.h"
#include "texture.h"

namespace phi
{
    namespace shaderStage
    {
        enum shaderStage
        {
            vertex,
            tesselationControl,
            tesselationEvaluation,
            geometry,
            fragment,
            compute
        };
    }

    class shader
    {
    private:
        shaderStage::shaderStage _stage;
        GLuint _id;
        string _content;
        string _fileName;

    private:
        shaderStage::shaderStage getStage(const string& fileName);
        string load(const string& fileName);
        shaderCompilationResult validate();

    public:
        static constexpr const char* VERT_EXT = ".vert";
        static constexpr const char* FRAG_EXT = ".frag";

    public:
        RENDERING_API shader(const string& fileName);
        RENDERING_API ~shader();
        RENDERING_API shaderCompilationResult compile();

        shaderStage::shaderStage getStage() const { return _stage; }
        GLuint getId() const { return _id; }
        string getContent() const { return _content; }
    };
}