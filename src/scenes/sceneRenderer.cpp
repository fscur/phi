#include "phi/scenes/sceneRenderer.h"

namespace phi
{
    sceneRenderer::sceneRenderer()
    {
        _defaultFrameBuffer = new defaultFrameBuffer(color::black);
    }

    sceneRenderer::~sceneRenderer()
    {
        safeDelete(_defaultFrameBuffer);
    }

    void sceneRenderer::createShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        _shader = shaderManager::get()->loadShader("BASIC_GEOM_PASS", "basic_geom_pass.vert", "basic_geom_pass.frag", attribs);

        _shader->addUniform(BASIC_SHADER::MVP, "mvp");
        _shader->addUniform(BASIC_SHADER::DIFFUSE_MAP, "diffuseMap");
        _shader->addUniform(BASIC_SHADER::DIFFUSE_COLOR, "diffuseColor");
    }

    void sceneRenderer::init()
    {
        _defaultFrameBuffer->init();
        createShader();
    }

    void sceneRenderer::render(scene* scene)
    {
        _scene = scene;
        _camera = _scene->getCamera();

        auto renderList = _scene->getRenderList();

        _defaultFrameBuffer->clear();
        
        auto p = _camera->getProjectionMatrix();
        auto v = _camera->getViewMatrix();
        auto vp = p * v;

        _shader->bind();

        for (auto pair : renderList)
        {
            auto material = pair.first;

            _shader->setUniform(BASIC_SHADER::DIFFUSE_MAP, material->getDiffuseTexture(), 0);
            _shader->setUniform(BASIC_SHADER::DIFFUSE_COLOR, material->getDiffuseColor());

            for (auto object : pair.second)
            {
                auto mvp = vp * object->getModelMatrix();
                _shader->setUniform(BASIC_SHADER::MVP, mvp);

                auto geometry = object->getGeometry();
                geometry->bind();
                geometry->render(); 
                geometry->unbind();
            }
        }

        _shader->unbind();
    }
}