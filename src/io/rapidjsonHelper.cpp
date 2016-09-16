#include <precompiled.h>
#include "rapidjsonHelper.h"

namespace phi
{
    rapidjson::Value rapidjsonHelper::getVec3JsonValue(vec3 vec, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value val(rapidjson::kArrayType);
        
        val.PushBack(vec.x, allocator);
        val.PushBack(vec.y, allocator);
        val.PushBack(vec.z, allocator);

        return val;
    }

    rapidjson::Value rapidjsonHelper::getQuatJsonValue(quat quat, rapidjson::Document::AllocatorType& allocator)
    {
        rapidjson::Value val(rapidjson::kArrayType);

        val.PushBack(quat.x, allocator);
        val.PushBack(quat.y, allocator);
        val.PushBack(quat.z, allocator);
        val.PushBack(quat.w, allocator);

        return val;
    }

    string rapidjsonHelper::getDocumentString(const rapidjson::Document& doc)
    {
        rapidjson::StringBuffer buffer;
        buffer.Clear();
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        
        doc.Accept(writer);

        return buffer.GetString();
    }
}