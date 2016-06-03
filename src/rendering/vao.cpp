#include <precompiled.h>
#include "vao.h"

namespace phi
{
    vao::vao()
    {
    }

    vao::~vao()
    {
        safeDelete(_vbo);
        safeDelete(_ebo);
    }

    void vao::bind()
    {
        glBindVertexArray(_id);
    }
}