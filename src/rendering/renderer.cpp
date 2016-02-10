#include "renderer.h"

namespace phi
{
    void renderer::render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _pipeline->vao->objectsCount, 0);
    }
}