#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "program.h"

namespace phi
{
    class liveShaderReloader
    {
    private:
        static std::map<string, program*> _programs;

    public:
        
        RENDERING_API static void reloadAllPrograms();
		RENDERING_API static void reloadProgram(string name);
    };
}