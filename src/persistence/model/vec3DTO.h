#pragma once

#include <core/transform.h>

namespace demon
{
    struct vec3DTO
    {
        vec3DTO(){}

        vec3DTO(phi::vec3 vec)
        {
            vector[0] = vec.x;
            vector[1] = vec.y;
            vector[2] = vec.z;
        }

        phi::vec3 toVec3() const
        {
            return phi::vec3(
                vector[0],
                vector[1],
                vector[2]);
        }

        phi::vector<float> vector = phi::vector<float>(3);
    };
}