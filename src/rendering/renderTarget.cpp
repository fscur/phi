#include <phi/core/globals.h>
#include <phi/rendering/renderTarget.h>

namespace phi
{
    renderTarget::renderTarget(std::string name, texture* texture)
    {
        _name = name;
        _texture = texture;
    }

    renderTarget::~renderTarget()
    {
        _texture->release();
        safeDelete(_texture);
    }
}