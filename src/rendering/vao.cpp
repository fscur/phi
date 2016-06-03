#include <precompiled.h>
#include "vao.h"

namespace phi
{
    vao::vao() :
        _id(0)
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

    void vao::unbind()
    {
        glBindVertexArray(0);
    }
}