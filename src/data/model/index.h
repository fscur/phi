#pragma once
#include <phi.h>

#include <core/guid.h>
#include <core/base64.h>

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/json.hpp>

namespace phi
{
    class index
    {
    public:
        struct indexEntry
        {
            guid guid;
            string path;

            template <typename Archive>
            void save(Archive& archive) const
            {
                archive(
                    cereal::make_nvp("Guid", guid.toStringBase64()),
                    cereal::make_nvp("Path", path)
                );
            }

            template <typename Archive>
            void load(Archive& archive)
            {
                string guidString;
                archive(
                    cereal::make_nvp("Guid", guidString),
                    cereal::make_nvp("Path", path)
                );

                auto guidBytes = base64::decode(guidString);
                guid = phi::guid(guidBytes);
            }
        };

    public:
        vector<indexEntry> entries;

    public:
        index() {};
        ~index() {};

        template <typename Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("Entries", entries));
        }

        template <typename Archive>
        void load(Archive& archive)
        {
            archive(cereal::make_nvp("Entries", entries));
        }
    };
}