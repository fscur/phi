#pragma once
#include <phi.h>
#include "ioApi.h"

namespace phi
{
    class IO_API rapidjsonHelper
    {
    public:
        static rapidjson::Value getVec3JsonValue(
            vec3 vec,
            rapidjson::Document::AllocatorType& allocator);
        static rapidjson::Value getQuatJsonValue(
            quat quat,
            rapidjson::Document::AllocatorType& allocator);
        static string getDocumentString(const rapidjson::Document& doc);
        static vec3 iteratorToVec3(rapidjson::GenericMemberIterator<true, rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>> memberIterator);
        static vec4 iteratorToVec4(rapidjson::GenericMemberIterator<true, rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>> memberIterator);
        static quat iteratorToQuat(rapidjson::GenericMemberIterator<true, rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>> memberIterator);
    };
}