#include "cursor.h"
#include <core\globals.h>

#include <loader\importer.h>

namespace phi
{
    cursor::cursor(std::string path, vec2 hotPoint)
    {
        phi::resource<texture>* textRes = nullptr;
        phi::importer::importTexture(path, textRes);

        _texture = textRes->getObject();
        _hotPoint = hotPoint;
    }

    cursor::~cursor()
    {
        _texture->releaseFromGpu();
        safeDelete(_texture);
    }
}