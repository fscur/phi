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
            vector.push_back(vec.x);
            vector.push_back(vec.y);
            vector.push_back(vec.z);
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