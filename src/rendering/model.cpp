#include <phi/core/globals.h>
#include <phi/io/path.h>
#include <phi/rendering/geometry.h>
#include <phi/rendering/model.h>
#include <phi/rendering/mesh.h>
#include <fstream>
#include <iostream>

namespace phi
{
    model::model(std::string name) : object3D(name, objectType::MODEL)
    {
    }
}