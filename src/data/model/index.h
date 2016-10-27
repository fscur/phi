#pragma once
#include <phi.h>

#include <core/exception.h>
#include <core/guid.h>
#include <core/base64.h>

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
        index() {};
        ~index() {};

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Entries", _entries));
        }

        indexEntry getEntryByIndex(int index)
        {
            return _entries[index];
        }

        bool contains(const guid& id)
        {
            for (auto& entry : _entries)
            {
                if (entry.guid == id)
                    return true;
            }

            return false;
        }

        indexEntry getEntryById(const guid& id)
        {
            for (auto& entry : _entries)
            {
                if (entry.guid == id)
                    return entry;
            }

            throw exception("Index entry not found");
        }

        static index load(string filePath)
        {
            std::ifstream inputFileStream(filePath);
            cereal::JSONInputArchive archive(inputFileStream);

            index index;
            index.serialize(archive);

            return index;
        }

    private:
        vector<indexEntry> _entries;
    };
}