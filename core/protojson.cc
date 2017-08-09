#include <iostream>
#include "protojson.h"
#include "core/assert.h"
#include "core/str.h"

// todo: add proto-> json
// todo: add error path
// todo: fix naming

namespace
{
  std::string json2field(const rapidjson::Value* json, google::protobuf::Message* msg, const google::protobuf::FieldDescriptor *field)
  {
    const auto* ref = msg->GetReflection();
    const bool repeated = field->is_repeated();
    switch (field->cpp_type())
    {
      case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
      {
        if (json->GetType() != rapidjson::kNumberType)
        {
          return "Not a number";
        }
        if (repeated)
        {
          ref->AddInt32(msg, field, (int32_t) json->GetInt());
        }
        else
        {
          ref->SetInt32(msg, field, (int32_t) json->GetInt());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
      {
        if (json->GetType() != rapidjson::kNumberType)
        {
          return "Not a number";
        }
        if (repeated)
        {
          ref->AddUInt32(msg, field, json->GetUint());
        }
        else
        {
          ref->SetUInt32(msg, field, json->GetUint());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
      {
        if (json->GetType() != rapidjson::kNumberType)
        {
          return "Not a number";
        }
        if (repeated)
        {
          ref->AddInt64(msg, field, json->GetInt64());
        }
        else
        {
          ref->SetInt64(msg, field, json->GetInt64());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
      {
        if (json->GetType() != rapidjson::kNumberType)
        {
          return "Not a number";
        }
        if (repeated)
        {
          ref->AddUInt64(msg, field, json->GetUint64());
        }
        else
        {
          ref->SetUInt64(msg, field, json->GetUint64());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
      {
        if (json->GetType() != rapidjson::kNumberType)
        {
          return "Not a number";
        }
        if (repeated)
        {
          ref->AddDouble(msg, field, json->GetDouble());
        }
        else
        {
          ref->SetDouble(msg, field, json->GetDouble());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
      {
        if (json->GetType() != rapidjson::kNumberType)
        {
          return "Not a number";
        }
        if (repeated)
        {
          ref->AddFloat(msg, field, json->GetDouble());
        }
        else
        {
          ref->SetFloat(msg, field, json->GetDouble());
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
      {
        if (json->GetType() != rapidjson::kTrueType && json->GetType() != rapidjson::kFalseType)
        {
          return "Not a bool";
        }
        bool v = json->GetBool();
        if (repeated)
        {
          ref->AddBool(msg, field, v);
        }
        else
        {
          ref->SetBool(msg, field, v);
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
      {
        if (json->GetType() != rapidjson::kStringType)
        {
          return "Not a string";
        }
        const char* value = json->GetString();
        uint32_t str_size = json->GetStringLength();
        std::string str_value(value, str_size);
        if (field->type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
        {
          if (repeated)
          {
            Assert(false);
            // ref->AddString(msg, field, b64_decode(str_value));
          }
          else
          {
            Assert(false);
            // ref->SetString(msg, field, b64_decode(str_value));
          }
        }
        else
        {
          if (repeated)
          {
            ref->AddString(msg, field, str_value);
          }
          else
          {
            ref->SetString(msg, field, str_value);
          }
        }
        break;
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
      {
        google::protobuf::Message *mf = (repeated) ? ref->AddMessage(msg, field) : ref->MutableMessage(msg, field);
        return protojson::ToProto(*json, mf);
      }
      case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
      {
        const auto* ed = field->enum_type();
        const google::protobuf::EnumValueDescriptor* ev = nullptr;
        if (json->GetType() == rapidjson::kNumberType)
        {
          ev = ed->FindValueByNumber(json->GetInt());
        }
        else if (json->GetType() == rapidjson::kStringType)
        {
          ev = ed->FindValueByName(json->GetString());
        }
        else return "Not an integer or string";
        if (!ev)
          return "Enum value not found";
        if (repeated)
        {
          ref->AddEnum(msg, field, ev);
        }
        else
        {
          ref->SetEnum(msg, field, ev);
        }
        break;
      }
      default:
        break;
    }
    return "";
  }
}

namespace protojson
{
  std::string ToProto(const rapidjson::Value& json, google::protobuf::Message* msg)
  {
    if (json.GetType() != rapidjson::kObjectType)
    {
      return "JSON root is not a object";
    }
    const  auto* d = msg->GetDescriptor();
    const auto* ref = msg->GetReflection();
    if (d == nullptr || ref == nullptr)
    {
      return "Protobuf object has no reflection";
    }
    
    for (rapidjson::Value::ConstMemberIterator itr = json.MemberBegin(); itr != json.MemberEnd(); ++itr)
    {
      const char* name = itr->name.GetString();
      const auto* field = d->FindFieldByName(name);
      if (field == nullptr)
        field = ref->FindKnownExtensionByName(name);
      if (field == nullptr) {
        return Str() << "Failed to find json field " << name << " in proto";
      }
      if (itr->value.GetType() == rapidjson::kNullType) {
        ref->ClearField(msg, field);
        continue;
      }
      if (field->is_repeated())
      {
        if (itr->value.GetType() != rapidjson::kArrayType) {
          return "Protobuf is repeated but json didnt specify an array";
        }
        for (rapidjson::Value::ConstValueIterator ait = itr->value.Begin(); ait != itr->value.End(); ++ait)
        {
          const auto ret = json2field(ait, msg, field);
          if (!ret.empty())
          {
            return ret;
          }
        }
      }
      else
      {
        const auto ret = json2field(&(itr->value), msg, field);
        if (!ret.empty())
        {
          return ret;
        }
      }
    }
    return "";
  }
}
