#include <precompiled.h>
#include "cursor.h"

#include <loader\importer.h>

namespace phi
{
    cursor::cursor(std::string path, vec2 hotPoint)
    {
        //TODO: Find a way to fix the resource delete and the object it contains
        //TODO: try catch this shit
        resource<texture>* textRes = importer::importTexture(path);

        _texture = textRes->getObject();
        _hotPoint = hotPoint;
    }

    cursor::~cursor()
    {
        //_texture->releaseFromGpu();
        safeDelete(_texture);
    }
}