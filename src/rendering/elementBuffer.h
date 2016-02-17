#pragma once

#include <GL\glew.h>
namespace phi
{
    class elementBuffer :
        public buffer
    {
    public:
        elementBuffer() : buffer(bufferTarget::element)
        {
        }

        ~elementBuffer()
        {
            buffer::~buffer();
        }
    };
}
