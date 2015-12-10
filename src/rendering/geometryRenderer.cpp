#include <phi/rendering/geometryRenderer.h>

namespace phi
{
    geometry* geometryRenderer::_lastGeometry = nullptr;

    void geometryRenderer::render(geometry* geometry)
    {
        if (geometry != _lastGeometry)
        {
            if (_lastGeometry != nullptr)
                _lastGeometry->unbind();

            geometry->bind();
            _lastGeometry = geometry;
        }

        geometry->render();
    }
}