#include <precompiled.h>
#include "scene.h"
#include <rendering\materialGpuData.h>
#include <rendering\materialInstance.h>
#include <rendering\renderInstance.h>

#include "sceneId.h"

namespace phi
{
    scene::scene(gl* gl, float w, float h) :
        _gl(gl),
        _renderer(new renderer(gl, w, h)),
        _pipeline(new pipeline(gl)),
        _camera(new camera("mainCamera", w, h, 0.1f, 1000.0f, PI_OVER_4)),
        _objects(vector<node*>()),
        _w(w),
        _h(h)
    {
        auto cameraNode = new node();
        cameraNode->addComponent(_camera);
        add(cameraNode);
    }

    scene::~scene()
    {
        for (auto object : _objects)
            safeDelete(object);

        safeDelete(_pipeline);
        safeDelete(_renderer);
    }

    void scene::update()
    {
        auto frameUniform = frameUniformBlock();
        frameUniform.p = _camera->getProjectionMatrix();
        frameUniform.v = _camera->getViewMatrix();
        frameUniform.vp = frameUniform.p * frameUniform.v;
        frameUniform.ip = glm::inverse(frameUniform.p);

        _pipeline->update(frameUniform);

        //TODO: Design flaw. Remove this slow copy assignment
        _renderer->getGBufferRenderPass()->setBatches(_pipeline->batches);
        _renderer->update();
    }

    void scene::render()
    {
        _renderer->render();
    }

    void scene::resize(float w, float h)
    {
        _camera->setWidth(w);
        _camera->setHeight(h);
    }

    void scene::uploadMaterial(material* material)
    {
        static uint currentId = 0;
        if (!phi::contains(_materialsCache, material))
            _materialsCache[material] = currentId++;

        auto albedoTextureAddress = _gl->texturesManager->add(material->albedoTexture);
        auto normalTextureAddress = _gl->texturesManager->add(material->normalTexture);
        auto specularTextureAddress = _gl->texturesManager->add(material->specularTexture);
        auto emissiveTextureAddress = _gl->texturesManager->add(material->emissiveTexture);

        auto materialGpuData = phi::materialGpuData(
            albedoTextureAddress.unit,
            normalTextureAddress.unit,
            specularTextureAddress.unit,
            emissiveTextureAddress.unit,
            albedoTextureAddress.page,
            normalTextureAddress.page,
            specularTextureAddress.page,
            emissiveTextureAddress.page,
            material->albedoColor,
            material->specularColor,
            material->emissiveColor,
            material->shininess,
            material->reflectivity,
            material->emission,
            material->opacity);

        auto offset = _materialsCache[material] * sizeof(phi::materialGpuData);

        auto instance = materialInstance(materialGpuData, offset);
        _pipeline->add(instance);
    }

    void scene::add(node* node)
    {
        node->traverseNodesContaining<mesh>([&](phi::node* currentNode, mesh* mesh)
        {
            sceneId::setNextId(mesh);

            if (!phi::contains(_materialsCache, mesh->material))
                uploadMaterial(mesh->material);

            renderInstance instance = { 0 };
            instance.mesh = mesh;
            instance.modelMatrix = currentNode->getTransform()->getModelMatrix();
            instance.materialId = _materialsCache[mesh->material];

            _pipeline->add(instance);
        });

        _objects.push_back(node);
    }

    void scene::remove(node* node)
    {
        phi::removeIfContains(_objects, node);

        //_pipeline->remove(node);

        node->traverse<mesh>([&](mesh* mesh)
        {
            sceneId::removeMeshId(mesh);
        });

        auto parent = node->getParent();
        if (parent)
            parent->removeChild(node);
    }

    mesh* scene::pick(int mouseX, int mouseY)
    {
        auto pixels = _renderer->getGBufferRenderPass()->getFramebuffer()->readPixels(
            _renderer->getGBufferRenderPass()->rt3,
            static_cast<GLint>(mouseX),
            static_cast<GLint>(_h - mouseY),
            1, 1); // What a shitty shit, right ?

        auto r = static_cast<int>(pixels.r);
        auto g = static_cast<int>(pixels.g) << 8;
        auto b = static_cast<int>(pixels.b) << 16;

        auto id = r | g | b;

        mesh* mesh = nullptr;
        if (id)
            mesh = sceneId::getMesh(id);

        return mesh;
    }

    inline float scene::getZBufferValue(int x, int y)
    {
        return _renderer->getDefaultFramebuffer()->getZBufferValue(x, y);
    }
}