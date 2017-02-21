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
            float width,
            float height,
            float fov,
            float near,
            float far,
            vec3DTO translation,
            vec3DTO scale,
            quatDTO rotation) :
            width(width),
            height(height),
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
                make_nvp("width", width),
                make_nvp("height", height),
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
                camera->getResolution().width,
                camera->getResolution().height,
                camera->getFov(),
                camera->getNear(),
                camera->getFar(),
                transform->getLocalPosition(),
                transform->getLocalSize(),
                transform->getLocalOrientation());
        }

        static phi::camera* to(const cameraDTO& cameraDto)
        {
            auto transform = new phi::transform();
            transform->setLocalPosition(cameraDto.translation.toVec3());
            transform->setLocalSize(cameraDto.scale.toVec3());
            transform->setLocalOrientation(cameraDto.rotation.toQuaternion());

            return new phi::camera(
                phi::resolution(cameraDto.width, cameraDto.height),
                transform,
                cameraDto.near,
                cameraDto.far,
                cameraDto.fov);
        }

        float width;
        float height;
        float fov;
        float near;
        float far;
        vec3DTO translation;
        vec3DTO scale;
        quatDTO rotation;
    };
}