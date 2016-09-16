#include <precompiled.h>
#include "rapidjsonHelper.h"

namespace phi
{
    using namespace rapidjson;

    Value rapidjsonHelper::getVec3JsonValue(vec3 vec, Document::AllocatorType& allocator)
    {
        Value val(kArrayType);

        val.PushBack(vec.x, allocator);
        val.PushBack(vec.y, allocator);
        val.PushBack(vec.z, allocator);

        return val;
    }

    Value rapidjsonHelper::getQuatJsonValue(quat quat, Document::AllocatorType& allocator)
    {
        Value val(kArrayType);

        val.PushBack(quat.x, allocator);
        val.PushBack(quat.y, allocator);
        val.PushBack(quat.z, allocator);
        val.PushBack(quat.w, allocator);

        return val;
    }

    string rapidjsonHelper::getDocumentString(const Document& doc)
    {
        StringBuffer buffer;
        buffer.Clear();
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        doc.Accept(writer);

        return buffer.GetString();
    }

    vec3 rapidjsonHelper::iteratorToVec3(GenericMemberIterator<true, UTF8<>, MemoryPoolAllocator<>> memberIterator)
    {
        return vec3(
            static_cast<float>(memberIterator->value[0].GetDouble()),
            static_cast<float>(memberIterator->value[1].GetDouble()),
            static_cast<float>(memberIterator->value[2].GetDouble()));
    }

    vec4 rapidjsonHelper::iteratorToVec4(GenericMemberIterator<true, UTF8<>, MemoryPoolAllocator<>> memberIterator)
    {
        return vec4(
            static_cast<float>(memberIterator->value[0].GetDouble()),
            static_cast<float>(memberIterator->value[1].GetDouble()),
            static_cast<float>(memberIterator->value[2].GetDouble()),
            static_cast<float>(memberIterator->value[3].GetDouble()));
    }

    quat rapidjsonHelper::iteratorToQuat(rapidjson::GenericMemberIterator<true, rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<>> memberIterator)
    {
        return quat(
            static_cast<float>(memberIterator->value[3].GetDouble()),
            static_cast<float>(memberIterator->value[0].GetDouble()),
            static_cast<float>(memberIterator->value[1].GetDouble()),
            static_cast<float>(memberIterator->value[2].GetDouble()));
    }
}