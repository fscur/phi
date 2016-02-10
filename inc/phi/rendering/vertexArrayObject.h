#ifndef _PHI_VERTEX_ARRAY_OBJECT_H_
#define _PHI_VERTEX_ARRAY_OBJECT_H_

#include "vertexBuffer.h"
#include "elementBuffer.h"

#include <vector>

namespace phi
{
    class vertexArrayObject
    {
    private:
        GLuint _id;
        std::vector<vertexBuffer*> _buffers;
        elementBuffer* _ebo;
    public:
        GLuint objectsCount;
    public:
        vertexArrayObject(uint objectsCount) :
            objectsCount(objectsCount)
        {
            glCreateVertexArrays(1, &_id);
        }

        ~vertexArrayObject()
        {
            glDeleteVertexArrays(1, &_id);
        }

        void bind()
        {
            glBindVertexArray(_id);
        }

        void unbind()
        {
            glBindVertexArray(0);
        }

        void createVbo(void* data, GLsizeiptr size);
        void createMaterialsIdsBuffer(void* data, GLsizeiptr size);
        void createModelMatricesBuffer(void* data, GLsizeiptr size);
        void createEbo(void* data, GLsizeiptr size);
    };
}
#endif