#pragma once
#include <phi.h>

namespace demon
{
    struct quatDTO
    {
        quatDTO(){}

        quatDTO(phi::quat quaternion)
        {
            vector[0] = quaternion.x;
            vector[1] = quaternion.y;
            vector[2] = quaternion.z;
            vector[3] = quaternion.w;
        }

        phi::quat toQuaternion()
        {
            return phi::quat(
                vector[0],
                vector[1],
                vector[2],
                vector[3]);
        }

        phi::vector<float> vector = phi::vector<float>(4);
    };
}