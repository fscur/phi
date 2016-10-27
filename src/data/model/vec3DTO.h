#pragma once
#include <phi.h>

namespace phi
{
    struct vec3DTO
    {
        vector<float> vector;

        vec3DTO() {}

        vec3DTO(vec3 vec)
        {
            vector[0] = vec.x;
            vector[1] = vec.y;
            vector[2] = vec.z;
        }

        vec3 toVec3()
        {
            return vec3(
                vector[0],
                vector[1],
                vector[2]
            );
        }
    };
}