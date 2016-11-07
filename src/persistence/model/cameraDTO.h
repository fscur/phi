#pragma once
#include <phi.h>

#include <core/transform.h>
#include <rendering/camera.h>

#include "vec3DTO.h"
#include "quatDTO.h"

namespace demon
{
    struct cameraDTO
    {
    public:
        cameraDTO() {}
        cameraDTO(
            float fov,
            float near,
            float far,
            vec3DTO translation,
            vec3DTO scale,
            quatDTO rotation) :
            fov(fov),
            near(near),
            far(far),
            translation(translation),
            scale(scale),
            rotation(rotation)
        {
        }

        ~cameraDTO()
        {
        }

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(
                make_nvp("fov", fov),
                make_nvp("near", near),
                make_nvp("far", far),
                make_nvp("translation", translation.vector),
                make_nvp("scale", scale.vector),
                make_nvp("rotation", rotation.vector)
            );
        }

        static cameraDTO from(phi::camera* camera)
        {
            auto transform = camera->getTransform();

            return cameraDTO(
                camera->getFov(),
                camera->getNear(),
                camera->getFar(),
                transform->getLocalPosition(),
                transform->getLocalSize(),
                transform->getLocalOrientation());
        }

        float fov;
        float near;
        float far;
        vec3DTO translation;
        vec3DTO scale;
        quatDTO rotation;
    };
}