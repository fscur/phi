#include "phi/ui/cursor.h"
#include "phi/core/globals.h"

namespace phi
{
    cursor::cursor(std::string name, std::string path, glm::vec2 hotPoint) :
        resource(name, path)
    {
        _fullName = name;
        _texture = texture::fromFile(path);
        _hotPoint = hotPoint;
    }

    cursor::~cursor()
    {
        _texture->release();
        safeDelete(_texture);
    }
}