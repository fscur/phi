#include "renderingSystem.h"
#include "shaderManager.h"

namespace phi
{
    renderingSystemInfo renderingSystem::info;
    defaultFrameBuffer* renderingSystem::defaultFrameBuffer = nullptr;
    pickingFrameBuffer* renderingSystem::pickingFrameBuffer = nullptr;
    FT_Library renderingSystem::freeTypeLibrary = nullptr;

    bool renderingSystem::initialized = false;

    void renderingSystem::init(renderingSystemInfo info)
    {
        if (FT_Init_FreeType(&renderingSystem::freeTypeLibrary))
            log("Could not init freetype library");

        renderingSystem::info = info;
        defaultFrameBuffer = new phi::defaultFrameBuffer(info.size, color::white);
        defaultFrameBuffer->init();

        pickingFrameBuffer = new phi::pickingFrameBuffer(info.size);
        pickingFrameBuffer->init();

        shaderManagerInfo shaderInfo;
        shaderInfo.path = info.resourcesPath;
        shaderManager::get()->init(shaderInfo);

        initialized = true;
    }

    void renderingSystem::release()
    {
        safeDelete(defaultFrameBuffer);
    }

    void renderingSystem::resize(sizef viewportSize)
    {
        info.size = viewportSize;
        defaultFrameBuffer->setSize(viewportSize);
        defaultFrameBuffer->setViewport(0, 0, viewportSize);

        pickingFrameBuffer = new phi::pickingFrameBuffer(viewportSize);
        pickingFrameBuffer->init();
    }
}