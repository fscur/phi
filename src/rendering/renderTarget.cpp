#include "renderTarget.h"

#include <core/globals.h>

namespace phi
{
    renderTarget::renderTarget(std::string name, texture* texture)
    {
        _name = name;
        _texture = texture;
    }

    renderTarget::~renderTarget()
    {
        _texture->releaseFromGpu();
        safeDelete(_texture);
    }
}