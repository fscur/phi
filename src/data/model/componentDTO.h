#pragma once
#include <phi.h>

#include <core/guid.h>

namespace phi
{
    struct componentDTO
    {
        string name;
        int type;
        string animatedThumbnailFileName;
        string thumbnailFileName;
        guid materialResourceGuid;
        guid geometryResourceGuid;

        template <typename Archive>
        void save(Archive& archive) const
        {
            if (type == 0)
            {
                archive(
                    make_nvp("Name", name),
                    make_nvp("Type", type),
                    make_nvp("AnimatedThumbnailFileName", animatedThumbnailFileName),
                    make_nvp("ThumbnailFileName", thumbnailFileName)
                );
            }
            else if (type == 1)
            {
                archive(
                    make_nvp("Name", name),
                    make_nvp("Type", type),
                    make_nvp("MaterialResourceGuid", materialResourceGuid),
                    make_nvp("GeometryResourceGuid", geometryResourceGuid)
                );
            }
        }

        template <typename Archive>
        void load(Archive& archive)
        {
            while (true)
            {
                const auto namePtr = archive.getNodeName();
                if (!namePtr)
                    break;

                const auto nodeName = string(namePtr);

                if (nodeName == "Name")
                    archive(make_nvp(nodeName, name));
                else if (nodeName == "Type")
                    archive(make_nvp(nodeName, type));
                else if (nodeName == "AnimatedThumbnailFileName")
                    archive(make_nvp(nodeName, animatedThumbnailFileName));
                else if (nodeName == "ThumbnailFileName")
                    archive(make_nvp(nodeName, thumbnailFileName));
                else if (nodeName == "MaterialResourceGuid")
                    archive(make_nvp(nodeName, materialResourceGuid));
                else if (nodeName == "GeometryResourceGuid")
                    archive(make_nvp(nodeName, geometryResourceGuid));
            }
        }
    };
}