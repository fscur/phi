#include "sceneMesh.h"

namespace phi
{
    void sceneMesh::setMaterial(material* value)
    { 
        _material = value; 

        if (_material != nullptr)
            _materialName = value->getName(); 
    }
}