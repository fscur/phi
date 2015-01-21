#include "globals.h"
#include "cursor.h"

namespace phi
{
    cursor::cursor(std::string name, std::string path, glm::vec2 hotPoint) :
        resource(name, path)
    {
        _texture = texture::fromFile(path);
        _hotPoint = hotPoint;
    }

    cursor::~cursor()
    {
        _texture->release();
        DELETE(_texture);
    }
}