#include <phi/core/globals.h>
#include <phi/rendering/renderingSystem.h>
#include <phi/rendering/texture.h>
#include <phi/rendering/material.h>
#include <phi/rendering/mesh.h>
#include <phi/rendering/shaderManager.h>

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
            LOG("Could not init freetype library");

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
        DELETE(defaultFrameBuffer);
    }

    void renderingSystem::resize(size<GLuint> viewportSize)
    {
        info.size = viewportSize;
        defaultFrameBuffer->setSize(info.size);
        defaultFrameBuffer->setViewport(0, 0, info.size);

        pickingFrameBuffer = new phi::pickingFrameBuffer(info.size);
        pickingFrameBuffer->init();
    }
}