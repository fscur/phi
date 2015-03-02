#include "renderingSystem.h"
#include "globals.h"
#include "texture.h"
#include "material.h"
#include "mesh.h"
#include "shaderManager.h"

namespace phi
{
    renderingSystemInfo renderingSystem::info;
    defaultFrameBuffer* renderingSystem::defaultFrameBuffer = nullptr;
    pickingFrameBuffer* renderingSystem::pickingFrameBuffer = nullptr;
    resourcesRepository* renderingSystem::repository = nullptr;
    FT_Library renderingSystem::freeTypeLibrary = nullptr;
    bool renderingSystem::initialized = false;

    void renderingSystem::init(renderingSystemInfo info)
    {
        if (FT_Init_FreeType(&renderingSystem::freeTypeLibrary))
            LOG("Could not init freetype library");

        renderingSystem::info = info;
        defaultFrameBuffer = new phi::defaultFrameBuffer(info.size, color::black);
        defaultFrameBuffer->init();
        defaultFrameBuffer->bind();

        defaultFrameBuffer->enable(GL_CULL_FACE);
        defaultFrameBuffer->enable(GL_DEPTH_TEST);

        pickingFrameBuffer = new phi::pickingFrameBuffer(info.size);
        pickingFrameBuffer->init();

        repository = new resourcesRepository();

        shaderManagerInfo shaderInfo;
        shaderInfo.path = info.applicationPath;
        shaderManager::get()->init(shaderInfo);

        repository = new resourcesRepository();
        initTextures();
        initMaterials();
        initMeshes();

        initialized = true;
    }

    void renderingSystem::initTextures()
    {
        std::string texturesPath = info.applicationPath + RENDERING_TEXTURES_PATH;
    }

    void renderingSystem::initMaterials()
    {
        //material* defaultMat = new material("default", "",
        //    "diffuse", //diffuse texture
        //    "normal", //normal texture
        //    "specular", //specular texture,
        //    color::white, // ambient color
        //    color::white, //diffuse color
        //    color::gray, //specular color
        //    0.2f, // ambient reflectance coeff
        //    0.8f, // diffuse reflectance coeff
        //    0.1f, // specular reflectance coeff
        //    6.5f);

        //repository->getResource<texture>(

        //repository->addResource(); // shininess

        //repository->addResource(new material("red", "",
        //    nullptr, //diffuse texture
        //    nullptr, //normal texture
        //    nullptr, //specular texture,
        //    color::red, // ambient color
        //    color::red, //diffuse color
        //    color::red, //specular color
        //    0.2f, // ambient reflectance coeff
        //    0.8f, // diffuse reflectance coeff
        //    0.0f, // specular reflectance coeff
        //    0.0f)); // shininess

        //repository->addResource(new material("blue", "",
        //    nullptr, //diffuse texture
        //    nullptr, //normal texture
        //    nullptr, //specular texture,
        //    color::blue, // ambient color
        //    color::blue, //diffuse color
        //    color::blue, //specular color
        //    0.2f, // ambient reflectance coeff
        //    0.8f, // diffuse reflectance coeff
        //    4.0f, // specular reflectance coeff
        //    64.0f)); // shininess

        //repository->addResource(new material("white", "",
        //    nullptr, //diffuse texture
        //    nullptr, //normal texture
        //    nullptr, //specular texture,
        //    color::white, // ambient color
        //    color::white, //diffuse color
        //    color::white, //specular color
        //    0.2f, // ambient reflectance coeff
        //    0.8f, // diffuse reflectance coeff
        //    0.0f, // specular reflectance coeff
        //    0.0f)); // shininess

        //repository->addResource(new material("bubble", "",
        //    repository->getResource<texture>("BubbleGrip-ColorMap"), //diffuse texture
        //    repository->getResource<texture>("BubbleGrip-NormalMap"), //normal texture
        //    nullptr, //specular texture,
        //    color::red, // ambient color
        //    color::orange, //diffuse color
        //    color::orange, //specular color
        //    0.2f, // ambient reflectance coeff
        //    0.3f, // diffuse reflectance coeff
        //    4.8f, // specular reflectance coeff
        //    8.0f)); // shininess
    }

    void renderingSystem::initMeshes()
    {
    /*    std::string meshesPath = info.applicationPath + RENDERING_MESHES_PATH;
        repository->addResource(mesh::fromObj("box", meshesPath + "cube.model"));*/
    }

    void renderingSystem::release()
    {
        DELETE(defaultFrameBuffer);
        repository->release();
        DELETE(repository);
    }

    void renderingSystem::resize(size<GLuint> viewportSize)
    {
        info.size = viewportSize;
        defaultFrameBuffer->setSize(info.size);
        defaultFrameBuffer->setViewport(0, 0, info.size);
    }
}