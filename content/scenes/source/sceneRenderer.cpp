#include "sceneRenderer.h"

namespace phi
{
    sceneRenderer::sceneRenderer(size<GLuint> viewportSize)
    {
        _viewportSize = viewportSize;
    }

    sceneRenderer::~sceneRenderer()
    {
    }

    void sceneRenderer::render(scene* scene)
    {
        _scene = scene;

        _allObjects = new std::vector<sceneObject*>();

        auto allObjects = scene->getAllObjects();
        for (auto i = 0; i < allObjects->size();i++)
        {
            if (allObjects->at(i)->getActive())
                _allObjects->push_back(allObjects->at(i));
        }

        //_allObjects = _scene->getAllObjects();
        _allObjectsCount = _allObjects->size();
        _camera = _scene->getActiveCamera();

        renderingSystem::defaultFrameBuffer->clear();

        onRender();
    }

    color sceneRenderer::getSelectionColor(int objectId, int meshId, bool selected)
    {
        //objectId max = 4095
        //meshId max = 1048575

        unsigned int id = objectId << 20;
        id += meshId;

        unsigned int r = id & 255;
        id = id >> 8;
        unsigned int g = id & 255;
        id = id >> 8;
        unsigned int b = id & 255;

        return color((float)r/255.0f, (float)g/255.0f, (float)b/255.0f, selected ? 1.0f : 0.0f);
    }
}