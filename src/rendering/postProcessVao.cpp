#include <precompiled.h>
#include "postProcessVao.h"

#include <core\geometry.h>

namespace phi
{
    postProcessVao::postProcessVao()
    {
        create();
    }

    postProcessVao::~postProcessVao()
    {
    }

    void postProcessVao::create()
    {
        auto quad = geometry::createQuad(2.0f);

        glCreateVertexArrays(1, &_id);
        glError::check();

        glBindVertexArray(_id);
        glError::check();

        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));

        _vbo = new vertexBuffer<vertex>(attribs);
        _vbo->storage(quad->vboSize, quad->vboData, bufferStorageUsage::write);

        _ebo = new buffer<uint>(bufferTarget::element);
        _ebo->storage(quad->eboSize, quad->eboData, bufferStorageUsage::write);

        glBindVertexArray(0);
        glError::check();
    }
}