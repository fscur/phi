#include <precompiled.h>
#include "material.h"

namespace phi
{
    material* material::defaultMaterial = material::createDefaultMaterial();

    inline material * material::createDefaultMaterial()
    {
        return new material(
            image::defaultAlbedoImage,
            image::defaultNormalImage,
            image::defaultSpecularImage,
            image::defaultEmissiveImage,
            vec3(1.0f),
            vec3(1.0f),
            vec3(1.0f),
            0.1f,
            0.0f,
            0.0f,
            1.0f);
    }
}