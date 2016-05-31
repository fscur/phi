#include <precompiled.h>
#include "liveShaderReloader.h"

#include <io\path.h>

namespace phi
{
    void liveShaderReloader::reloadShader(string fileName)
    {
        if (phi::contains(shaders, fileName))
        {
            auto shader = shaders[fileName];
            shader->setIsDirty();
        }
    }
}