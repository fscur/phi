#include "phi/scenes/sceneRenderer.h"

namespace phi 
{
    sceneRenderer::sceneRenderer(sizef size)
    {
        _defaultFrameBuffer = new defaultFrameBuffer(size, color::black);
        _hasBindlessTextures = false;
    }

    sceneRenderer::~sceneRenderer()
    {
        safeDelete(_defaultFrameBuffer);
    }

    void sceneRenderer::init()
    {
        initExtensions();
        _defaultFrameBuffer->init();
        createShader();
    }

    void sceneRenderer::initExtensions()
    {
        const GLubyte* extension = nullptr;
        auto i = 0;
        extension = glGetStringi(GL_EXTENSIONS, i++);

        auto extensionsAvailable = std::vector<GLubyte*>();
        auto bindless = false;
        while (extension != NULL)
        {
            auto ex = std::string((char*)extension);

            if (ex == "GL_ARB_bindless_texture")
            {
                _hasBindlessTextures = true;
                break;
            }

            extension = glGetStringi(GL_EXTENSIONS, i++);
        }

        if (_hasBindlessTextures)
            phi::log("BINDLESS SHIT RIGHT HERE WOOOOOOWWWW C:");
        else
            phi::log("DOES NOT HAVE BINDLESS SHIT :C");
    }

    void sceneRenderer::createShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        _shader = shaderManager::get()->loadShader("BASIC_GEOM_PASS", "basic_geom_pass.vert", "basic_geom_pass.frag", attribs);

        _shader->addUniform(BASIC_SHADER::MVP, "mvp");
        _shader->addUniform(BASIC_SHADER::DIFFUSE_TEXTURE_ARRAY_INDEX, "diffuseTextureArrayIndex");
        _shader->addUniform(BASIC_SHADER::DIFFUSE_TEXTURE_PAGE_INDEX, "diffuseTexturePageIndex");
        _shader->addUniform(BASIC_SHADER::TEXTURE_ARRAYS, "textureArrays");
        _shader->addUniform(BASIC_SHADER::DIFFUSE_COLOR, "diffuseColor");
    }

    void sceneRenderer::render(scene* scene)
    {
        _scene = scene;
        _camera = _scene->getCamera();

        auto renderList = _scene->getRenderList();
        auto textureStorageDatas = _scene->getTextureStorageDatas();

        _defaultFrameBuffer->clear();

        auto p = _camera->getProjectionMatrix();
        auto v = _camera->getViewMatrix();
        auto vp = p * v;

        _shader->bind();

        auto units = _scene->getTextureArrayUnits();
        _shader->setUniform(BASIC_SHADER::TEXTURE_ARRAYS, units);

        for (auto materials : renderList)
        {
            auto material = materials.first;
            auto tex = material->getDiffuseTexture();
            auto texStorageData = textureStorageDatas[tex];

            _shader->setUniform(BASIC_SHADER::DIFFUSE_TEXTURE_ARRAY_INDEX, texStorageData.arrayIndex);
            _shader->setUniform(BASIC_SHADER::DIFFUSE_TEXTURE_PAGE_INDEX, (GLfloat)texStorageData.pageIndex);
            _shader->setUniform(BASIC_SHADER::DIFFUSE_COLOR, material->getDiffuseColor());

            for (auto geometries : materials.second)
            {
                auto geometry = geometries.first;
                geometry->bind();

                for (auto mesh : geometries.second)
                {
                    auto mvp = vp * mesh->getModelMatrix();
                    _shader->setUniform(BASIC_SHADER::MVP, mvp);

                    geometry->render();
                }

                geometry->unbind();
            }
        }

        _shader->unbind();
    }
}