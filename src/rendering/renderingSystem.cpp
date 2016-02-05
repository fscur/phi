#include <phi/core/globals.h>
#include <phi/rendering/renderingSystem.h>
#include <phi/rendering/shaderManager.h>

namespace phi
{
    renderingSystemInfo renderingSystem::info;
    FT_Library renderingSystem::freeTypeLibrary = nullptr;

    bool renderingSystem::initialized = false;

    void renderingSystem::init(renderingSystemInfo info)
    {
        if (FT_Init_FreeType(&renderingSystem::freeTypeLibrary))
            log("Could not init freetype library");

        renderingSystem::info = info;

        shaderManagerInfo shaderInfo;
        shaderInfo.path = info.resourcesPath;
        shaderManager::get()->init(shaderInfo);

        initialized = true;
    }

    void renderingSystem::release()
    {
    }

    void renderingSystem::resize(sizef viewportSize)
    {
        info.size = viewportSize;
    }
}