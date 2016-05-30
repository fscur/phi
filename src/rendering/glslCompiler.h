#pragma once
#include <phi.h>
#include <io\path.h>

namespace phi
{
    class glslCompiler
    {
    private:
        static EShLanguage getLanguage(const string& name);
        static string getDataType(int type);
        static TBuiltInResource createResources();
        static glslang::TShader* parse(EShLanguage language, const string fileName);

    public:
        static glslang::TProgram compile(const string vertFile, const string fragFile);
    };
}