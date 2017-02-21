#pragma once
#include <phi.h>

#include <core/node.h>
#include <core/guid.h>

#include "guidDTO.h"
#include "vec3DTO.h"
#include "quatDTO.h"

namespace demon
{
    struct nodeDTO
    {
    public:
        nodeDTO(){}
        nodeDTO(
            phi::string name,
            guidDTO guid,
            vec3DTO translation,
            vec3DTO scale,
            quatDTO rotation) :
            name(name),
            guid(guid),
            translation(translation),
            scale(scale),
            rotation(rotation)
        {
        }

        ~nodeDTO()
        {
        }

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(
                make_nvp("guid", guid),
                make_nvp("name", name),
                make_nvp("translation", translation.vector),
                make_nvp("scale", scale.vector),
                make_nvp("rotation", rotation.vector)
            );
        }

        static nodeDTO from(phi::model* model)
        {
            auto guid = model->getGuid();
            auto node = model->getNode();
            auto transform = node->getTransform();
            auto name = node->getName();

            return nodeDTO(
                name,
                guid,
                transform->getLocalPosition(),
                transform->getLocalSize(),
                transform->getLocalOrientation());
        }

        phi::string name;
        guidDTO guid;
        vec3DTO translation;
        vec3DTO scale;
        quatDTO rotation;
    };
}